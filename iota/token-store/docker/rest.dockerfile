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
