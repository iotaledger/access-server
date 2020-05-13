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
# * Makefile for ASN authentication tests
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

use_tinyembedded = false

# Set for cross compile
SYSROOT ?= /
TOOL_PREFIX ?= /usr/bin/

CC = $(TOOL_PREFIX)gcc
LD = $(TOOL_PREFIX)ld

BINARY_MAIN = main

OBJS_MAIN = main.o

INCLUDE = include/
ACCESS = ../access/
NETWORK = ../network/
PLUGINS = ../plugins/
MODULE = ./modules/
EXTERN = ./CpuGeneric/
ASN = $(NETWORK)asn/

ifeq ($(use_tinyembedded),true)
ASN_IMPL = $(ASN)implementations/tinyembedded/
ASN_IMPL_LDFLAGS =
else
ASN_IMPL = $(ASN)implementations/libCrypto/
ASN_IMPL_LDFLAGS = -lcrypto
endif

ZLIB = $(EXTERN)zlib-1.2.11/
JSMN = $(EXTERN)jsmn/
CRYPTO = $(EXTERN)crypto/
PIP = $(ACCESS)pip/
PEP = $(ACCESS)pep/
PDP = $(ACCESS)pdp/
PAP = $(ACCESS)pap/
STORAGE = $(PLUGINS)storage/
RESOLVER = $(PLUGINS)resolver/
AWS_DAEMON = $(MODULE)AWS_daemon/
TCP_SERVER = $(MODULE)tcp_server/
DATA_ACQUISITION = $(PLUGINS)data/acquisition/
VEHICLE_DATASET = $(PLUGINS)data/dataset/vehicle/
USER_MANAGEMENT = $(PAP)user/
GPS = $(DATA_ACQUISITION)gps/
CAN = $(DATA_ACQUISITION)can/
OBDII = $(DATA_ACQUISITION)obdii/
MODBUS = $(DATA_ACQUISITION)modbus/
CANOPEN = $(DATA_ACQUISITION)canopen/
FASTJSON = $(EXTERN)libfastjson/

CURL_LDFLAGS = -lcurl
SQLITE_LDFLAGS = -lsqlite3

LDFLAGS = -Wl,-rpath-link,$(SYSROOT)/lib/arm-linux-gnueabihf,-rpath-link,$(SYSROOT)/usr/lib/arm-linux-gnueabihf --sysroot=$(SYSROOT) -L$(SYSROOT)/lib -L$(SYSROOT)/usr/lib -L$(SYSROOT)/usr/lib/arm-linux-gnueabihf -B$(SYSROOT)/usr/lib/arm-linux-gnueabihf -dynamic -lpthread -lpigpio

SRCS = \
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
    $(CRYPTO)ref/utils.c \
    $(ASN_IMPL)asn_utils.c \
    $(ASN_IMPL)asn_server.c \
    $(ASN_IMPL)asn_client.c \
    $(INCLUDE)time_manager.c \
    $(PIP)pip.c \
    $(PDP)pdp.c \
    $(PEP)pep.c \
    $(PAP)pap.c \
    $(JSMN)jsmn.c \
    $(JSMN)json_parser.c \
    $(ASN)asn_auth_helper.c \
    $(ASN)asn_auth.c \
    $(STORAGE)storage.c \
    $(AWS_DAEMON)psDaemon.c \
    $(RESOLVER)resolver.c \
    $(RESOLVER)relay_interface.c \
    $(RESOLVER)demos/demo_resolver_01.c \
    $(RESOLVER)demos/demo_resolver_02.c \
    include/json_interface.c \
    include/timer.c \
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
    $(VEHICLE_DATASET)vehicle_datasharing_dataset.c \
    $(NETWORK)network.c \
    $(USER_MANAGEMENT)user.c \
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
    -I$(ASN) \
    -I$(PEP) \
    -I$(ZLIB) \
    -I$(JSMN) \
    -I$(AWS_DAEMON) \
    -I$(STORAGE) \
    -I$(CRYPTO) \
    -I$(ASN_IMPL) \
    -I$(PIP) \
    -I$(PDP) \
    -I$(PAP) \
    -I$(CRYPTO)ref/ \
    -I$(RESOLVER) \
    -I$(RESOLVER)demos \
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
    -I$(FASTJSON) \
    -I$(NETWORK)

all: $(BINARY_MAIN) can_simulator

clean:
	@echo "Deleting all binaries..."
	@rm main can_simulator tests/*/*_test tests/*/*.o $(OBJS) $(OBJS_MAIN) -f

%.o: %.c
	@echo CC $@
	$(AT)$(CC) -c $(CFLAGS) $< -o $@

can_simulator: can_simulator.o $(CAN)can_linux.o $(CAN)lib.o config_manager/config_manager.o config_manager/ini/ini.o
	@echo LINK can_simulator
	$(AT)$(CC) -o can_simulator $^

$(BINARY_MAIN): $(OBJS_MAIN) $(OBJS)
	@echo LINK $(BINARY_MAIN)
	$(AT)$(CC) -o $(BINARY_MAIN) -g -O0 $(OBJS_MAIN) $(OBJS) $(LDFLAGS) $(CURL_LDFLAGS) $(SQLITE_LDFLAGS) $(ASN_IMPL_LDFLAGS)

relay_interface_test: tests/relay_interface/relay_interface_test.o $(RESOLVER)relay_interface.o
	@echo  LINK relay_interface_test
	$(AT)$(CC) -o tests/relay_interface/relay_interface_test -g -O0 $^ $(LDFLAGS)

user_management_test: tests/user_management/user_management_test tests/user_management/user_management_test.o $(JSMN)jsmn.o
	@echo LINK $(DAC_test)
	$(AT)$(CC) -o tests/user_management_test user_management/user_management_test.o $(JSMN)jsmn.o $(SQLITE_LDFLAGS) $(LDFLAGS) $(CURL_LDFLAGS)

asn_test: tests/asn_test/asn_test.o $(OBJS)
	@echo LINK tests/asn_test/asn_test
	$(AT)$(CC) -o tests/asn_test/asn_test tests/asn_test/asn_test.o $(OBJS) $(LDFLAGS) $(CURL_LDFLAGS) $(SQLITE_LDFLAGS) $(ASN_IMPL_LDFLAGS)

