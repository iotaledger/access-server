FROM node:10

RUN mkdir -p /home/node/app/node_modules

RUN mkdir -p /home/node/app/config

WORKDIR /home/node/app

VOLUME /home/node/app/data

COPY package*.json ./

ENV NODE_ENV production

ENV NODE_CONFIG_DIR /home/node/app/config/

RUN npm install

COPY . .

ENTRYPOINT [ "node", "dist/iota_server.js" ]
