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

FROM node:10

RUN mkdir -p /home/node/app/node_modules

VOLUME /home/node/app/config/

WORKDIR /home/node/app

COPY package*.json ./

ENV NODE_ENV production

ENV NODE_CONFIG_DIR /home/node/app/config/

RUN npm install

COPY . .

ENTRYPOINT [ "node", "dist/bc_http_server.js" ]
