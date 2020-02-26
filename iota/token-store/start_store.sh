#!/usr/bin/env bash

source .env

docker-compose \
    -f docker-compose.yml \
    run -d \
    --service-ports \
    --name ts-iota-rest \
    ts-iota-rest
