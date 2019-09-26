#!/usr/bin/env bash

docker-compose -f docker-compose-bc-deploy.yml build bc-ps-deployer
docker-compose -f docker-compose-bc-deploy.yml run bc-ps-deployer
docker-compose -f docker-compose-bc-deploy.yml rm -f
