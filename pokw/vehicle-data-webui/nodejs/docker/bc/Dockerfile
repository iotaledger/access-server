FROM node:10

RUN mkdir -p /home/node/app/node_modules

RUN mkdir -p /home/node/app/config/bc

RUN mkdir /home/node/app/data

WORKDIR /home/node/app

COPY package*.json ./

ENV NODE_ENV production

ENV NODE_CONFIG_DIR /home/node/app/config/bc

RUN npm install

COPY . .

ENTRYPOINT [ "node", "dist/bc_server.js" ]