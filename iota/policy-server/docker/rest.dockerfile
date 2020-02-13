FROM node:10.19.0-alpine3.11

RUN mkdir -p /home/node/app/node_modules /home/node/app/config

WORKDIR /home/node/app

ENV NODE_ENV=production NODE_CONFIG_DIR=/home/node/app/config

COPY package*.json ./

RUN npm install

COPY . .

ENTRYPOINT [ "node", "dist/rest_server.js" ]