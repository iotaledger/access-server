#
#  This file is part of the DAC distribution (https://github.com/xainag/frost)
#  Copyright (c) 2019 XAIN AG.
#
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, version 3.
#
#  This program is distributed in the hope that it will be useful, but
#  WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
#  General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program. If not, see <http://www.gnu.org/licenses/>.
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
