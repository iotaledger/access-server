#!/usr/bin/env python3

#
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
#

import ipfshttpclient
import io
import tempfile
import socket
import select
import sys
import requests
import json
import simplejson
import signal
import threading
import os

from web3.shh import Shh
from web3 import Web3, HTTPProvider

import configparser

config = configparser.ConfigParser()
curdir = os.path.dirname(os.path.realpath(__file__))
config.read("{}/../config.ini".format(curdir))

if "config" in config and "policy_store_service_ip" in config["config"]:
    blockchain_ip = config["config"]["policy_store_service_ip"]
else:
    blockchain_ip = "34.77.82.182"
blockchain_port = 8454

if "config" in config and "whisper_pubkey" in config["config"]:
    pubkey = config["config"]["whisper_pubkey"]
else:
    pubkey = "0x0476a7fe325709fe6420b216d7fb3bea263b012f44bfd5a00a1c7e125f0cfe2b220d6d1a6e5f0ed87ef39953d4a19969cf7d100cd52546b0a970ae0956001b83ce"

def signal_handler(sig, frame):
    print("User initiated exit...")
    if ipfsuploader_thread:
        ipfsuploader_thread.stop()

class SocketWaiter(threading.Thread):
    def __init__(self, ipfsapi_instance=None, web3_instance=None, address=("127.0.0.1", 12345)):
        threading.Thread.__init__(self)
        self.address = address
        self.ipfsapi_instance = ipfsapi_instance
        self.web3_instance = web3_instance
        self.running = True

    def run(self):
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            s.setblocking(0)
            s.bind(self.address)
            while self.running:
                data_recvd = False
                s.listen()

                inputs = [s]
                outputs = []
                ready_to_read, ready_to_write, in_error = select.select(inputs, outputs, inputs, 0.1)

                if len(ready_to_read) > 0:
                    conn, addr = ready_to_read[0].accept()
                    data = bytearray()
                    with conn:
                        while True:
                            tmp_data = conn.recv(1024)
                            if not tmp_data:
                                break
                            data += tmp_data
                    data_recvd = True

                if data_recvd == False:
                    continue
                my_json = simplejson.dumps(
                    json.loads(data.decode("utf8").replace("'", '"')), ignore_nan=True
                )
                rethash = self.ipfsapi_instance.add_json(json.loads(my_json))
                print("hash of newly added file: {}".format(rethash))

                self.web3_instance.shh.post(
                    {
                        "payload": self.web3_instance.toHex(text=rethash),
                        "pubkey": pubkey,
                        "topic": "0x12345678",
                        "powTarget": 12.5,
                        "powTime": 10,
                    }
                )

    def stop(self):
        self.running = False

if __name__ == "__main__":
    signal.signal(signal.SIGINT, signal_handler)
    api = ipfshttpclient.connect("/ip4/127.0.0.1/tcp/5001/http")

    web3 = Web3(HTTPProvider("http://{}:{}".format(blockchain_ip, blockchain_port)))
    Shh.attach(web3, "shh")

    ipfsuploader_thread = SocketWaiter(address=("127.0.0.1", 12345), ipfsapi_instance=api, web3_instance=web3)
    ipfsuploader_thread.start()
    ipfsuploader_thread.join()
