FROM raspbian/stretch

RUN apt-get update && apt-get upgrade -q -y && apt-get install -y -q libfastjson4 libcurl3 libsqlite3-0 wiringpi

WORKDIR /delegation_server/

COPY delegation_srvr ./
COPY config.ini ./

ENTRYPOINT [ "./delegation_srvr" ]
