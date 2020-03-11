#
# This file is part of the Frost distribution
# (https://github.com/iotaledger/frost)
# 
# Copyright (c) 2020 IOTA.
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

## Stage image
FROM node:10 AS base

RUN mkdir -p /home/node/app/node_modules

WORKDIR /home/node/app

COPY package*.json ./

RUN npm install

COPY . .

RUN node_modules/.bin/tsc


## Final image
FROM node:10.19.0-alpine3.11

RUN mkdir -p /home/node/app/node_modules /home/node/app/dist

WORKDIR /home/node/app

COPY protos protos
COPY --from=base /home/node/app/dist ./dist/
COPY --from=base /home/node/app/package*.json ./

ENV NODE_ENV=production

RUN npm install

ENTRYPOINT [ "node", "dist/rest_server.js" ]
