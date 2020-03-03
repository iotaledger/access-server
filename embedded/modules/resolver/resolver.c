/*
 * This file is part of the Frost distribution
 * (https://github.com/xainag/frost)
 *
 * Copyright (c) 2019 XAIN AG.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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

#define BOARD_INDICATION
#define CAR_RELAY

unsigned int flashing_counter[MAX_LED_NUMBER]={0};
unsigned int on_counter[MAX_LED_NUMBER]={0};

static char relayboard_addr[128] = "127.0.0.1";

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

int policy_update_indication()
{
    //Dlog_printf("policy_update_indication\n");

    return 0;
}
