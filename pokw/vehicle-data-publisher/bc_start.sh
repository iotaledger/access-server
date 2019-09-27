#!/usr/bin/env bash

docker-compose -p bc-vd-publisher -f docker-compose-bc.yml up -d $@
