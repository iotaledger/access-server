############################################################################
# This file is part of the Frost distribution
# (https://github.com/xainag/frost)
#
# Copyright (c) 2019 XAIN AG.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
############################################################################

#/****************************************************************************
# * \project Decentralized Access Control
# * \file Makefile
# * \brief
# * Makefile for dac authentication tests
# *
# * @Author Dejan Nedic
# *
# * \notes
# *
# * \history
# * 14.08.2018. Initial version.
# ****************************************************************************/

ifdef V
AT_0 := @
AT_1 :=
AT = $(AT_$(V))
else
AT = @
endif

# Set for cross compile
SYSROOT ?= /
TOOL_PREFIX ?= /usr/bin/

CC = $(TOOL_PREFIX)gcc
LD = $(TOOL_PREFIX)ld

BINARY_SERVER = serverTest
BINARY_CLIENT = clientTest

OBJS_SERVER = test-server.o
OBJS_CLIENT = test-client.o

INCLUDE = include/
MODULE = ./modules/
EXTERN = ./CpuGeneric/
LIBDAC = $(MODULE)libdac/
TINYE = $(LIBDAC)tinyembedded/
ZLIB = $(EXTERN)zlib-1.2.11/
JSMN = $(EXTERN)jsmn/
CRYPTO = $(EXTERN)crypto/
PIP = $(MODULE)pip/
PEP = $(MODULE)pep/
PDP = $(MODULE)pdp/
POLICYSTORE = $(INCLUDE)policyStore/
RESOLVER = $(MODULE)resolver/
AWS_DAEMON = $(MODULE)AWS_daemon/
TCP_SERVER = $(MODULE)tcp_server/
DATA_ACQUISITION = data_acquisition_modules/
VEHICLE_DATASET = vehicle_dataset/
USER_MANAGEMENT = user_management/
GPS = $(DATA_ACQUISITION)gps/
CAN = $(DATA_ACQUISITION)can/
OBDII = $(DATA_ACQUISITION)obdii/
MODBUS = $(DATA_ACQUISITION)modbus/
CANOPEN = $(DATA_ACQUISITION)canopen/
FASTJSON = $(EXTERN)libfastjson/

CURL_LDFLAGS = -lcurl
SQLITE_LDFLAGS = -lsqlite3

LDFLAGS = -Wl,-rpath-link,$(SYSROOT)/lib/arm-linux-gnueabihf,-rpath-link,$(SYSROOT)/usr/lib/arm-linux-gnueabihf --sysroot=$(SYSROOT) -L$(SYSROOT)/lib -L$(SYSROOT)/usr/lib -L$(SYSROOT)/usr/lib/arm-linux-gnueabihf -B$(SYSROOT)/usr/lib/arm-linux-gnueabihf -dynamic -lpthread -lwiringPi -lwiringPiDev

SRCS = \
    $(INCLUDE)Dlog.c \
    $(INCLUDE)tcp_client.c \
    bc/bc_daemon.c \
    $(INCLUDE)utils_string.c \
    $(CRYPTO)sha512.c \
    $(CRYPTO)curve25519-donna.c \
    $(CRYPTO)aes.c \
    $(CRYPTO)sha256.c \
    $(CRYPTO)ref/ge25519.c \
    $(CRYPTO)ref/crypto_hash_sha512.c \
    $(CRYPTO)ref/fe25519.c \
    $(CRYPTO)ref/sc25519.c \
    $(CRYPTO)ref/keypair.c \
    $(CRYPTO)ref/open.c \
    $(CRYPTO)ref/sign.c \
    $(CRYPTO)ref/verify.c \
    $(TINYE)libdacUtils.c \
    $(TINYE)libdacServer.c \
    $(TINYE)libdacClient.c \
    $(INCLUDE)time_manager.c \
    $(PIP)pip.c \
    $(PDP)pdp.c \
    $(PEP)pep.c \
    $(JSMN)jsmn.c \
    $(JSMN)json_parser.c \
    $(INCLUDE)authDacHelper.c \
    $(LIBDAC)libauthdac.c \
    $(POLICYSTORE)policystore.c \
    $(AWS_DAEMON)psDaemon.c \
    $(RESOLVER)resolver.c \
    include/json_interface.c \
    config_manager/config_manager.c \
    config_manager/ini/ini.c \
    $(GPS)minmea/minmea.c \
    $(GPS)gps_receiver.c \
    $(OBDII)ejv_obdii/OBDII.c \
    $(OBDII)ejv_obdii/OBDIICommunication.c \
    $(OBDII)obdii_receiver.c \
    $(CAN)lib.c \
    $(CAN)can_linux.c \
    $(CAN)can_thread.c \
    $(CAN)can_msgs.c \
    $(CAN)can_receiver.c \
    $(CANOPEN)canopen_receiver.c \
    $(CANOPEN)canopen_sdo.c \
    $(VEHICLE_DATASET)vehicle_dataset.c \
    $(TCP_SERVER)tcp_server.c \
    user_management/user_management.c \
    $(MODBUS)modbus.c \
    $(MODBUS)modbus_receiver.c \
    $(FASTJSON)arraylist.c \
    $(FASTJSON)debug.c \
    $(FASTJSON)json_object.c \
    $(FASTJSON)json_object_iterator.c \
    $(FASTJSON)json_print.c \
    $(FASTJSON)json_tokener.c \
    $(FASTJSON)json_util.c \
    $(FASTJSON)json_version.c \
    $(FASTJSON)printbuf.c

OBJS =	$(SRCS:.c=.o)

CFLAGS = \
    -std=gnu99 \
    --sysroot=$(SYSROOT) \
    -I. \
    -I$(SYSROOT)/usr/include/arm-linux-gnueabihf/ \
    -I$(INCLUDE) \
    -I$(LIBDAC)include \
    -I$(PEP) \
    -I$(ZLIB) \
    -I$(JSMN) \
    -I$(AWS_DAEMON) \
    -I$(POLICYSTORE) \
    -I$(CRYPTO) \
    -I$(TINYE)include/ \
    -I$(PIP) \
    -I$(PDP) \
    -I$(CRYPTO)ref/ \
    -I$(RESOLVER) \
    -I$(CAN) \
    -I$(OBDII) \
    -I$(OBDII)ejv_obdii \
    -I$(GPS) \
    -I$(GPS)minmea \
    -I/usr/include/libfastjson \
    -I$(VEHICLE_DATASET) \
    -I$(CANOPEN) \
    -Ibc \
    -Iconfig_manager \
    -Iconfig_manager/klib \
    -Iconfig_manager/ini \
    -I$(VEHICLE_DATASET) \
    -I$(VEHICLE_DATASET)impls \
    -I$(TCP_SERVER) \
    -I$(USER_MANAGEMENT) \
    -I$(MODBUS) \
    -I$(FASTJSON)

all: $(BINARY_SERVER) docker_preparation can_simulator

clean:
	@echo "Deleting all binaries..."
	@rm serverTest clientTest can_simulator $(OBJS) $(OBJS_CLIENT) $(OBJS_SERVER) -f

%.o: %.c
	@echo CC $@
	$(AT)$(CC) -c $(CFLAGS) $< -o $@

user_mgmt_test: user_mgmt_test.o user_management/user_management.o $(JSMN)jsmn.o
	@echo LINK $(BINARY_CLIENT)
	$(AT)$(CC) -o user_mgmt_test user_mgmt_test.o user_management/user_management.o $(JSMN)jsmn.o $(SQLITE_LDFLAGS) $(LDFLAGS) $(CURL_LDFLAGS)

can_simulator: can_simulator.o $(CAN)can_linux.o $(CAN)lib.o config_manager/config_manager.o config_manager/ini/ini.o
	@echo LINK can_simulator
	$(AT)$(CC) -o can_simulator $^

$(BINARY_CLIENT): $(OBJS_CLIENT) $(OBJS)
	@echo LINK $(BINARY_CLIENT)
	$(AT)$(CC) -o $(BINARY_CLIENT) $(OBJS_CLIENT) $(OBJS) $(LDFLAGS) $(CURL_LDFLAGS) $(SQLITE_LDFLAGS)

$(BINARY_SERVER): $(OBJS_SERVER) $(OBJS)
	@echo LINK $(BINARY_SERVER)
	$(AT)$(CC) -o $(BINARY_SERVER) -g -O0 $(OBJS_SERVER) $(OBJS) $(LDFLAGS) $(CURL_LDFLAGS) $(SQLITE_LDFLAGS)

docker_preparation: $(BINARY_SERVER)
	cp $(BINARY_SERVER) docker/delegation_server/delegation_srvr
	cp bc_webui/*.py docker/ipfs_bc_if/
	cp config.ini docker/delegation_server/config.ini
	cp config.ini docker/ipfs_bc_if/config.ini
