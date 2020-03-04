/*
 * This file is part of the DAC distribution (https://github.com/xainag/frost)
 * Copyright (c) 2019 XAIN AG.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

/****************************************************************************
 * \project Decentralized Access Control
 * \file resolver.c
 * \brief
 * Implementation of Resolver for Raspberry Pi 3B+ board
 *
 * @Author Vladimir Vojnovic
 *
 * \notes
 *
 * \history
 * 03.10.2018. Initial version.
 * 28.02.2020. Added data sharing through action functionality
 ****************************************************************************/

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <strings.h>
#include <string.h>

//#include "board_led.h"
#include "resolver.h"
#include "Dlog.h"

#include <time.h>
#include <sys/time.h>
#include <stdlib.h>

#include <wiringPi.h>
#include <piFace.h>

#include "can_receiver.h"
#include "canopen_receiver.h"
#include "vehicle_datashering_dataset.h"
#include "vehicle_dataset.h"
#include "json_interface.h"
#include "timer.h"

#define BOARD_INDICATION
#define CAR_RELAY
#define DATASET_NUM_SIZE 2
#define DATASET_NUM_POSITION 9

unsigned int flashing_counter[MAX_LED_NUMBER]={0};
unsigned int on_counter[MAX_LED_NUMBER]={0};

static char relayboard_addr[128] = "127.0.0.1";
static int timerId = -1;

// piface stuff
#define PIFACE_BASE  64
#define PIFACE_BASE  64
#define PIFACE_LED   (PIFACE_BASE)

static int pifacerelayplus_initialized = 0;
static void pifacerelayplus_init_maybe(){
    if (pifacerelayplus_initialized == 0) {
        pifacerelayplus_initialized = 1;
        wiringPiSetupSys();
        piFaceSetup(PIFACE_BASE);
    }
}

int car_status=0;
void set_led_flashing(unsigned char led, unsigned int duration)
{
    flashing_counter[led]=duration;
}

void set_led_on(unsigned char led, unsigned int duration)
{
    on_counter[led]=duration;
}

int get_time(char *buf)
{
    time_t     now;
    struct tm  ts;
    //char       buf[80];

    // Get current time
    time(&now);

    // Format time, "ddd yyyy-mm-dd hh:mm:ss zzz"
    ts = *localtime(&now);
    strftime(buf, 80, "%H:%M:%S", &ts);
}


char action_s[] = "<Action performed>";

void Resolver_set_relayboard_addr(const char* addr)
{
    strncpy(relayboard_addr, addr, sizeof(relayboard_addr));
}

static void piface_pulse(int relay_idx) {
    printf("PIFACE PULSE\n");
    pifacerelayplus_init_maybe();
    digitalWrite(PIFACE_LED + relay_idx, HIGH);
    delay(500);
    digitalWrite(PIFACE_LED + relay_idx, LOW);
}

static int toggle_state[4] = {LOW, LOW, LOW, LOW};
static void piface_toggle(int relay_idx) {
    printf("PIFACE TOGGLE\n");
    pifacerelayplus_init_maybe();
    if (toggle_state[relay_idx] == LOW) {
        toggle_state[relay_idx] = HIGH;
        digitalWrite(PIFACE_LED + relay_idx, HIGH);
    } else if (toggle_state[relay_idx] == HIGH) {
        toggle_state[relay_idx] = LOW;
        digitalWrite(PIFACE_LED + relay_idx, LOW);
    }
}

static void piface_on(int relay_idx) {
    printf("PIFACE ON\n");
    pifacerelayplus_init_maybe();
    toggle_state[relay_idx] = HIGH;
    digitalWrite(PIFACE_LED + relay_idx, HIGH);
}

static void piface_off(int relay_idx) {
    printf("PIFACE OFF\n");
    pifacerelayplus_init_maybe();
    toggle_state[relay_idx] = LOW;
    digitalWrite(PIFACE_LED + relay_idx, LOW);
}

static void socket_send_byte_to_port(int portname)
{
    int sockfd, connfd;
    struct sockaddr_in servaddr, cli;
    char data = '1';

    // socket create and varification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("Socket creation failed... Not sending\n");
    }

    struct timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;

    if (setsockopt (sockfd, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout,
                    sizeof(timeout)) < 0)
        perror("Resolved setsockopt failed");

    bzero(&servaddr, sizeof(servaddr));

    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(relayboard_addr);
    servaddr.sin_port = htons(portname);

    if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0) {
        perror("Resolver connect failed");
    } else {
        write(sockfd, &data, 1);
    }

    close(sockfd);
}

static int can01_remote_tcp_car_lock() {
    socket_send_byte_to_port(2222);
    return 0;
}

static int can01_remote_tcp_car_unlock() {
    socket_send_byte_to_port(2223);
    return 0;
}

static int can01_remote_tcp_start_engine() {
    socket_send_byte_to_port(2224);
    return 0;
}

static int can01_remote_tcp_open_trunk() {
    socket_send_byte_to_port(2225);
    return 0;
}

static int can01_remote_car_lock() {
    piface_pulse(0);
    return 0;
}

static int can01_remote_car_unlock() {
    piface_pulse(1);
    return 0;
}

static int can01_remote_start_engine() {
    piface_pulse(2);
    return 0;
}

static int can01_remote_open_trunk() {
    piface_pulse(3);
    return 0;
}

static int canopen01_car_lock() {
    piface_off(3);
    return 0;
}

static int canopen01_car_unlock() {
    piface_on(3);
    return 0;
}

static int canopen01_start_engine() {
    piface_on(1);
    return 0;
}

static int canopen01_open_trunk() {
    piface_pulse(2);
    return 0;
}

static int canopen01_alarm_off() {
    piface_off(1);
    return 0;
}

static int (*res_action_02_p)() = can01_remote_car_lock;
static int (*res_action_01_p)() = can01_remote_car_unlock;
static int (*res_action_04_p)() = can01_remote_start_engine;
static int (*res_action_03_p)() = can01_remote_open_trunk;
static int (*res_action_05_p)() = canopen01_alarm_off;

void Resolver_init_can01_remote() {
    res_action_02_p = can01_remote_car_lock;
    res_action_01_p = can01_remote_car_unlock;
    res_action_03_p = can01_remote_open_trunk;
    res_action_04_p = can01_remote_start_engine;
}

void Resolver_init_can01_remote_tcp() {
    res_action_02_p = can01_remote_tcp_car_lock;
    res_action_01_p = can01_remote_tcp_car_unlock;
    res_action_03_p = can01_remote_tcp_open_trunk;
    res_action_04_p = can01_remote_tcp_start_engine;
}

void Resolver_init_canopen01() {
    res_action_02_p = canopen01_car_lock;
    res_action_01_p = canopen01_car_unlock;
    res_action_03_p = canopen01_open_trunk;
    res_action_04_p = canopen01_start_engine;
    res_action_05_p = canopen01_alarm_off;
}

int run_led()
{
    char       buf[80];

    get_time(buf);

    Dlog_printf("%s %s run led\n", buf, action_s);

    return 0;
}

void timer_handler(size_t timer_id, void * user_data)
{
    Resolver_action07();
}

int Resolver_action02()
{
    char       buf[80];

    get_time(buf);

    Dlog_printf("%s %s\tResolver action 2\n", buf, action_s);
    return res_action_02_p();
}

int Resolver_action01()
{
    char       buf[80];

    get_time(buf);

    Dlog_printf("%s %s\tResolver action 1\n", buf, action_s);
    return res_action_01_p();
}

int car_drive_relay()
{
    //Dlog_printf("car_drive_relay\n");

    return 0;
}

int Resolver_action04()
{
    char       buf[80];

    get_time(buf);

    Dlog_printf("%s %s\tResolver action 4\n", buf, action_s);
    return res_action_04_p();
}

int Resolver_action03()
{
    char       buf[80];

    get_time(buf);

    Dlog_printf("%s %s\tResolver action 3\n", buf, action_s);
    return res_action_03_p();
}

int Resolver_action05()
{
    char       buf[80];

    get_time(buf);

    Dlog_printf("%s %s\tResolver action 5\n", buf, action_s);
    return res_action_05_p();
}

int Resolver_action06(char *action, unsigned long end_time)
{
    static VehicleDataset_state_t vdstate = {0};

    if (action == NULL)
    {
        Dlog_printf("\nERROR[%s] - Wrong input parameter\n", __FUNCTION__);
        return -1;
    }

    // Initialize interface
    if (CanReceiver_isInUse())
    {
        char can0_port_name[MAX_STR_SIZE];
        char can1_port_name[MAX_STR_SIZE];

        CanReceiver_getBodyChannel(can0_port_name, MAX_STR_SIZE);
        CanReceiver_getChasChannel(can1_port_name, MAX_STR_SIZE);
#ifdef TINY_EMBEDDED
        CanReceiver_deinit();
        // Re-init receiver with new dataset
#endif

        Resolver_init_can01_remote();
        vdstate.options = &VehicleDatasetCan01_options[0];
        vdstate.dataset = (can01_vehicle_dataset_t*)malloc(sizeof(can01_vehicle_dataset_t));
        VehicleDataset_init(&vdstate);
        CanReceiver_init(can0_port_name, can1_port_name, vdstate.dataset, JSONInterface_get_mutex());
    }
    else if (CanopenReceiver_isInUse())
    {
        char canopen_port_name[MAX_STR_SIZE];
        int canopen_node_id;

        CanopenReceiver_getPortName(canopen_port_name, MAX_STR_SIZE);
        canopen_node_id = CanopenReceiver_getNodeId();
#ifdef TINY_EMBEDDED
        CanopenReceiver_deinit();
        // Re-init receiver with new dataset
#endif

        Resolver_init_canopen01();
        vdstate.options = &VehicleDatasetCanopen01_options[0];
        vdstate.dataset = (canopen01_vehicle_dataset_t*)malloc(sizeof(canopen01_vehicle_dataset_t));
        VehicleDataset_init(&vdstate);
        CanopenReceiver_init(vdstate.dataset, JSONInterface_get_mutex(), canopen_port_name, canopen_node_id);
    }

    // Chose dataset
    char* dataset;
    char dataset_num[DATASET_NUM_SIZE];

    memcpy(dataset_num, &action[DATASET_NUM_POSITION], DATASET_NUM_SIZE);

    switch(atoi(dataset_num))
    {
        case 1:
            dataset = VehicleDataset01_options;
            break;
        case 2:
            dataset = VehicleDataset02_options;
            break;
        case 3:
            dataset = VehicleDataset03_options;
            break;
        case 4:
            dataset = VehicleDataset04_options;
            break;
        case 5:
            dataset = VehicleDataset05_options;
            break;
        case 6:
            dataset = VehicleDataset06_options;
            break;
        case 7:
            dataset = VehicleDataset07_options;
            break;
        case 8:
            dataset = VehicleDataset08_options;
            break;
        case 9:
            dataset = VehicleDataset09_options;
            break;
        case 10:
            dataset = VehicleDataset10_options;
            break;
        case 11:
            dataset = VehicleDataset11_options;
            break;
        case 12:
            dataset = VehicleDataset12_options;
            break;
        case 13:
            dataset = VehicleDataset13_options;
            break;
        case 14:
            dataset = VehicleDataset14_options;
            break;
        case 15:
            dataset = VehicleDataset15_options;
            break;
        case 16:
            dataset = VehicleDataset16_options;
            break;
        default:
            dataset = NULL;
            break;
    }

    // Tokenize dataset
    char* tok = NULL;

    tok = strtok(dataset, "|");

    while (tok != NULL)
    {
        for (int i = 0; i < vdstate.options_count; i++)
        {
            // Set options that needs to ne acquried
            if (strncmp(vdstate.tokens[i].name, tok, strlen(tok) < strlen(vdstate.tokens[i].name) ? strlen(tok) : strlen(vdstate.tokens[i].name)) == 0)
            {
                vdstate.tokens[i].val = 1;
            }
        }

        tok = strtok(NULL, "|");
    }

    unsigned char *dataset_uint8 = (unsigned char*)vdstate.dataset;

    for (int i = 0; i < vdstate.options_count; i ++)
    {
        dataset_uint8[i] = vdstate.tokens[i].val;
    }

    if (CanReceiver_isInUse())
    {
        CanReceiver_start();
        timerId = Timer_start(end_time - getEpochTime(), timer_handler, TIMER_SINGLE_SHOT, NULL);
    }
    else if (CanopenReceiver_isInUse())
    {
        CanopenReceiver_start();
    }

    return 0;
}

int Resolver_action07()
{
    Timer_stop(timerId);

    if (CanReceiver_isInUse())
    {
        CanReceiver_deinit();
    }
    else if (CanopenReceiver_isInUse())
    {
        CanopenReceiver_deinit();
    }

    return 0;
}

int policy_update_indication()
{
    //Dlog_printf("policy_update_indication\n");

    return 0;
}
