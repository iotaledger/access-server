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
 * \file gps_receiver.c
 * \brief
 * Implementation of interface for GPS module
 *
 * @Author Djordje Golubovic
 *
 * \notes
 *
 * \history
 * 04.15.2019. Initial version.
 ****************************************************************************/
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <signal.h>
#include <pthread.h>
#include <stdlib.h>

#include "minmea.h"
#include <libfastjson/json.h>

#include "json_interface.h"
#include "globals_declarations.h"

// GPS data stuff
static double latitude = -1.;
static double longitude = -1.;
static double speed = -1.;
static fjson_object* fj_obj_gps;
static fjson_object* fj_obj_location;
static fjson_object* gps_json_filler()
{
    fj_obj_gps = fjson_object_new_array();
    fj_obj_location = fjson_object_new_object();
    fjson_object_object_add(fj_obj_location, "name", fjson_object_new_string("location"));
    fjson_object_array_add(fj_obj_gps, fj_obj_location);
    return fj_obj_gps;
}
#define GPS_JSON_NAME "gps_data"
// !GPS data stuff

typedef struct {
    char portname[64];
    fjson_object* fj_root;
    pthread_mutex_t *json_mutex;
} GpsReceiver_thread_args_t;

static GpsReceiver_thread_args_t targs;

static int set_interface_attribs (int fd, int speed, int parity)
{
        struct termios tty;
        memset (&tty, 0, sizeof tty);
        if (tcgetattr (fd, &tty) != 0)
        {
                fprintf(stderr, "error %d from tcgetattr", errno);
                return -1;
        }

        cfsetospeed (&tty, speed);
        cfsetispeed (&tty, speed);

        tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
        // disable IGNBRK for mismatched speed tests; otherwise receive break
        // as \000 chars
        tty.c_iflag &= ~IGNBRK;         // disable break processing
        tty.c_lflag = 0;                // no signaling chars, no echo,
                                        // no canonical processing
        tty.c_oflag = 0;                // no remapping, no delays
        tty.c_cc[VMIN]  = 0;            // read doesn't block
        tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

        tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

        tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
                                        // enable reading
        tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
        tty.c_cflag |= parity;
        tty.c_cflag &= ~CSTOPB;
        tty.c_cflag &= ~CRTSCTS;

        if (tcsetattr (fd, TCSANOW, &tty) != 0)
        {
                fprintf(stderr, "error %d from tcsetattr", errno);
                return -1;
        }
        return 0;
}

static void set_blocking (int fd, int should_block)
{
    struct termios tty;
    memset (&tty, 0, sizeof tty);
    if (tcgetattr (fd, &tty) != 0)
    {
        fprintf(stderr, "error %d from tggetattr", errno);
        return;
    }

    tty.c_cc[VMIN]  = should_block ? 1 : 0;
    tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

    if (tcsetattr (fd, TCSANOW, &tty) != 0){
        fprintf(stderr, "error %d setting term attributes", errno);
    }
}

static char nmea_sentence_buffer[83];
static int nmea_sentence_next_idx = 0;

fjson_object *create_rmc_json_record(struct minmea_sentence_rmc *frame)
{
    char str[64] = "\0";
    fjson_object *fj_root = fjson_object_new_object();
    fjson_object *fj_sentence_id = fjson_object_new_string("rmc");
    fjson_object *fj_latitude = fjson_object_new_double(minmea_tocoord(&frame->latitude));
    fjson_object *fj_longitude = fjson_object_new_double(minmea_tocoord(&frame->longitude));
    fjson_object *fj_speed = fjson_object_new_double(minmea_tofloat(&frame->speed));
    fjson_object_object_add(fj_root, "sentence_id", fj_sentence_id);
    fjson_object_object_add(fj_root, "latitude", fj_latitude);
    fjson_object_object_add(fj_root, "longitude", fj_longitude);
    fjson_object_object_add(fj_root, "speed", fj_speed);

    return fj_root;
}

fjson_object *create_gsv_json_record(struct minmea_sentence_gsv *frame)
{
    fjson_object *fj_root = fjson_object_new_object();
    //fjson_object_object_add(fj_root, "sentence_id", fjson_object_new_string("gsv"));
    //fjson_object_object_add(fj_root, "msg_nr", fjson_object_new_int(frame->msg_nr));
    //fjson_object_object_add(fj_root, "total_msgs", fjson_object_new_int(frame->total_msgs));
    //fjson_object_object_add(fj_root, "total_sats", fjson_object_new_int(frame->total_sats));
    //fjson_object *fj_sat_array = fjson_object_new_array();
    //fjson_object_object_add(fj_root, "sats", fj_sat_array);
    //for (int i=0; i<frame->total_sats; i++)
    //{
    //    fjson_object *fj_sat_root = fjson_object_new_object();
    //    fjson_object_object_add(fj_sat_root, "nr", fjson_object_new_int(frame->sats[i].nr));
    //    fjson_object_object_add(fj_sat_root, "elevation", fjson_object_new_int(frame->sats[i].elevation));
    //    fjson_object_object_add(fj_sat_root, "azimuth", fjson_object_new_int(frame->sats[i].azimuth));
    //    fjson_object_object_add(fj_sat_root, "snr", fjson_object_new_int(frame->sats[i].snr));
    //    fjson_object_array_add(fj_sat_array, fj_sat_root);
    //}
    return fj_root;
}

fjson_object *create_gsa_json_record(struct minmea_sentence_gsa *frame)
{
    fjson_object *fj_root = fjson_object_new_object();
    fjson_object_object_add(fj_root, "sentence_id", fjson_object_new_string("gsa"));
    //fjson_object_object_add(fj_root, "pdop", fjson_object_new_double(minmea_tofloat(&frame->pdop)));
    //fjson_object_object_add(fj_root, "hdop", fjson_object_new_double(minmea_tofloat(&frame->hdop)));
    //fjson_object_object_add(fj_root, "vdop", fjson_object_new_double(minmea_tofloat(&frame->vdop)));

    return fj_root;
}

fjson_object *create_gga_json_record(struct minmea_sentence_gga *frame)
{
    char unit[64] = "\0\0";
    fjson_object *fj_root = fjson_object_new_object();
    fjson_object_object_add(fj_root, "latitude", fjson_object_new_double(minmea_tocoord(&frame->latitude)));
    fjson_object_object_add(fj_root, "longitude", fjson_object_new_double(minmea_tocoord(&frame->longitude)));
    fjson_object_object_add(fj_root, "time", fjson_object_new_string(unit));

    return fj_root;
}

fjson_object *create_processed_gps_json_record(double latitude, double longitude, double speed)
{
    fjson_object *fj_root = fjson_object_new_object();
    fjson_object_object_add(fj_root, "latitude", fjson_object_new_double(latitude));
    fjson_object_object_add(fj_root, "longitude", fjson_object_new_double(longitude));
    if (speed > 0.0) fjson_object_object_add(fj_root, "speed", fjson_object_new_double(speed));

    return fj_root;
}

static int end_thread = 0;
static pthread_t gps_thread = 0;

static void *gps_thread_loop(void *ptr)
{
    int fd = -1;
    //char *portname = (char*)ptr;
    GpsReceiver_thread_args_t *targs = (GpsReceiver_thread_args_t*)ptr;

    fd = open(targs->portname, O_RDONLY | O_NOCTTY | O_SYNC);

    if (fd < 0)
    {
        printf("Couldn't open serial port at %s\n", targs->portname);
        end_thread = 1;
    }

    set_interface_attribs (fd, B4800, 0);   // set speed to 4800 bps, 8n1 (no parity)
    set_blocking (fd, 1);                   // set blocking

    while (end_thread == 0) {
        char buffer[100];
        ssize_t length = read(fd, &buffer, sizeof(buffer));
        if (length == -1)
        {
            printf("Error reading from serial port\n");
            break;
        }
        else if (length == 0)
        {
            printf("No more data\n");
            break;
        }
        else
        {
            buffer[length] = '\0';
            char *token = strtok(buffer, "\n\r");
            int loc_idx = 0;
            while(token) {
                if (token[0] == '$' || token[0] == '!'){
                    // process previous sentence
                    int sentence_id = minmea_sentence_id(nmea_sentence_buffer, false);
                    switch (sentence_id)
                    {
                        case MINMEA_SENTENCE_GGA: {
                            struct minmea_sentence_gga frame;
                            if (minmea_parse_gga(&frame, nmea_sentence_buffer)) {
                                pthread_mutex_lock(targs->json_mutex);
                                if (frame.latitude.scale != 0 && frame.longitude.scale != 0) {
                                    fjson_object *fj_obj_tmp = fjson_object_new_object();
                                    fjson_object_object_add(fj_obj_tmp, "latitude", fjson_object_new_double(minmea_tocoord(&frame.latitude)));
                                    fjson_object_object_add(fj_obj_tmp, "longitude", fjson_object_new_double(minmea_tocoord(&frame.longitude)));
                                    fjson_object_object_add(fj_obj_location, "value", fj_obj_tmp);
                                    //fjson_object_object_add(fj_obj_gps, "speed", fjson_object_new_double(-1.));
                                } else {
                                    fjson_object *fj_obj_tmp = fjson_object_new_object();
                                    fjson_object_object_add(fj_obj_tmp, "latitude", fjson_object_new_double(0.0));
                                    fjson_object_object_add(fj_obj_tmp, "longitude", fjson_object_new_double(0.0));
                                    fjson_object_object_add(fj_obj_location, "value", fj_obj_tmp);
                                    //fjson_object_object_add(fj_obj_gps, "speed", fjson_object_new_double(-1.));
                                }
                                pthread_mutex_unlock(targs->json_mutex);
                            }
                        } break;
                        // case MINMEA_SENTENCE_GSA: {
                        //     struct minmea_sentence_gsa frame;
                        //     if (minmea_parse_gsa(&frame, nmea_sentence_buffer)) {
                        //         pthread_mutex_lock(targs->json_mutex);
                        //         fjson_object_array_add(fj_obj_gps, create_gsa_json_record(&frame));
                        //         pthread_mutex_unlock(targs->json_mutex);
                        //     }
                        // } break;
                        case MINMEA_SENTENCE_RMC: {
                            struct minmea_sentence_rmc frame;
                            if (minmea_parse_rmc(&frame, nmea_sentence_buffer)) {
                                pthread_mutex_lock(targs->json_mutex);
                                if (frame.latitude.scale != 0 && frame.longitude.scale != 0 && frame.speed.scale != 0) {
                                    fjson_object *fj_obj_tmp = fjson_object_new_object();
                                    fjson_object_object_add(fj_obj_tmp, "latitude", fjson_object_new_double(minmea_tocoord(&frame.latitude)));
                                    fjson_object_object_add(fj_obj_tmp, "longitude", fjson_object_new_double(minmea_tocoord(&frame.longitude)));
                                    fjson_object_object_add(fj_obj_location, "value", fj_obj_tmp);
                                    //fjson_object_object_add(fj_obj_gps, "speed", fjson_object_new_double(minmea_tofloat(&frame.speed)));
                                } else {
                                    fjson_object *fj_obj_tmp = fjson_object_new_object();
                                    fjson_object_object_add(fj_obj_tmp, "latitude", fjson_object_new_double(0.0));
                                    fjson_object_object_add(fj_obj_tmp, "longitude", fjson_object_new_double(0.0));
                                    fjson_object_object_add(fj_obj_location, "value", fj_obj_tmp);
                                    //fjson_object_object_add(fj_obj_gps, "speed", fjson_object_new_double(-1.));
                                }
                                pthread_mutex_unlock(targs->json_mutex);
                            }
                        } break;
                        // case MINMEA_SENTENCE_GSV: {
                        //     struct minmea_sentence_gsv frame;
                        //     if (minmea_parse_gsv(&frame, nmea_sentence_buffer)) {
                        //         pthread_mutex_lock(targs->json_mutex);
                        //         fjson_object_array_add(fj_obj_gps, create_gsv_json_record(&frame));
                        //         pthread_mutex_unlock(targs->json_mutex);
                        //     }
                        // } break;
                        default:
                            break;
                    }
                    // start collecting next sentence
                    nmea_sentence_next_idx = 0;
                    memset(nmea_sentence_buffer, 0, 83);
                    strncpy(nmea_sentence_buffer+nmea_sentence_next_idx, token, strlen(token));
                    nmea_sentence_next_idx += strlen(token);
                } else {
                    strncpy(nmea_sentence_buffer+nmea_sentence_next_idx , token, strlen(token));
                    nmea_sentence_next_idx += strlen(token);
                }
                token = strtok(NULL, "\n\r");
                JSONInterface_dump_if_needed(6, 1);
            }
        }
        usleep(g_task_sleep_time);
    }
}

int GpsReceiver_init(const char* portname, pthread_mutex_t *json_mutex)
{
    char buff[128] = {0};
    strncpy(targs.portname, portname, 64);

    JSONInterface_add_module_init_cb(gps_json_filler, &fj_obj_gps, GPS_JSON_NAME);

    targs.json_mutex = json_mutex;
}

int GpsReceiver_start()
{
    if (pthread_create(&gps_thread, NULL, gps_thread_loop, (void*)&targs)){
        fprintf(stderr, "Error creating GPS thread\n");
        return -2;
    }

    return 0;
}

int GpsReceiver_end()
{
    end_thread = 1;
    pthread_join(gps_thread, NULL);
}
