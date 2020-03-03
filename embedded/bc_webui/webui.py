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

import signal

from flask import Flask
from flask import render_template
from flask import jsonify
from flask import json
from flask_cors import CORS
from flask_cors import cross_origin

import time
import os

import ipfshttpclient
from web3.shh import Shh
from web3 import Web3, HTTPProvider

import ipfs

import configparser

config = configparser.ConfigParser()
curdir = os.path.dirname(os.path.realpath(__file__))
config.read("{}/../config.ini".format(curdir))

if "config" in config and "whisper_pubkey" in config["config"]:
    pubkey = config["config"]["whisper_pubkey"]
else:
    pubkey = "0x0476a7fe325709fe6420b216d7fb3bea263b012f44bfd5a00a1c7e125f0cfe2b220d6d1a6e5f0ed87ef39953d4a19969cf7d100cd52546b0a970ae0956001b83ce"

prvkey = "0x1a7ff2c05240eb36a580e03480454bd35a3077c7179c6644f12ce6dfaddbc6a6"

if "config" in config and "whisper_keypair_identity" in config["config"]:
    key_pair_identity = config["config"]["whisper_keypair_identity"]
else:
    key_pair_identity = "20b637d3d40cb74875765250e8ad693aceed234b500c02adec0e279f4daaf49a"

if "config" in config and "policy_store_service_ip" in config["config"]:
    blockchain_ip = config["config"]["policy_store_service_ip"]
else:
    blockchain_ip = "34.77.82.182"
blockchain_port = 8454

if "config" in config and "mainrpi_ipaddress" in config["config"]:
    mainrpi_ipaddress = config["config"]["mainrpi_ipaddress"]
else:
    mainrpi_ipaddress = "192.168.3.1"

webui_ip_port=5007
ipfs_server_address = "192.168.3.20"

def create_gunicorn():
    global ipfsuploader_thread

    app = Flask(__name__)
    CORS(app)
    ipfsuploader_thread.start()

    def signal_handler(sig, frame):
        global ipfsuploader_thread
        if ipfsuploader_thread:
            ipfsuploader_thread.stop()

    signal.signal(signal.SIGINT, signal_handler)

    return app

if __name__ == '__main__':
    api = ipfshttpclient.connect("/ip4/{}/tcp/5001/http".format(ipfs_server_address))
    web3 = Web3(HTTPProvider("http://{}:{}".format(blockchain_ip, blockchain_port)))
    Shh.attach(web3, "shh")

    filter_id = web3.shh.newMessageFilter({"topic": "0x12345678", "privateKeyID": key_pair_identity})

    app = Flask(__name__)
    CORS(app)
else:
    api = ipfshttpclient.connect("/ip4/{}/tcp/5001/http".format(ipfs_server_address))
    web3 = Web3(HTTPProvider("http://{}:{}".format(blockchain_ip, blockchain_port)))
    Shh.attach(web3, "shh")
    filter_id = web3.shh.newMessageFilter({"topic": "0x12345678", "privateKeyID": key_pair_identity})
    ipfsuploader_thread = ipfs.SocketWaiter(address=("0.0.0.0", 12345), ipfsapi_instance=api, web3_instance=web3)

    app = create_gunicorn()

@app.route("/get_new_messages")
def get_new_messages():
    new_messages = web3.shh.getMessages(filter_id.filter_id)
    return jsonify({"payloads": [x["payload"] for x in new_messages]})

@app.route("/cat/<ipfs_hash>")
def cat(ipfs_hash):
    return jsonify(json.loads(api.cat(ipfs_hash)))

last_recorded_hash = ""

@app.route("/cat_last")
@cross_origin(supports_credentials=True)
def cat_last():
    global last_recorded_hash
    new_messages = web3.shh.getMessages(filter_id.filter_id)
    if len(new_messages) > 0:
        last_recorded_hash = new_messages[-1]["payload"].decode()

    if last_recorded_hash == "":
        retval = {"error": "No msgs ever!"}
    else:
        retval = json.loads(api.cat(last_recorded_hash))

    return jsonify(retval)

if __name__ == '__main__':
    app.run(debug=True, host="0.0.0.0", port=webui_ip_port)
