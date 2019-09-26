#!/usr/bin/env bash

docker-compose -f docker-compose-bc-deploy.yml build
docker-compose -f docker-compose-bc-deploy.yml run deployer
docker-compose -f docker-compose-bc-deploy.yml rm -f