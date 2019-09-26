FROM raspbian/stretch

RUN apt-get update && apt-get upgrade -q -y && apt-get install -y -q python3 python3-pip
RUN pip3 install Flask
RUN pip3 install flask_cors
RUN pip3 install requests
RUN pip3 install eth-utils
RUN pip3 install eth-keyfile
RUN pip3 install rlp
RUN pip3 install web3
RUN pip3 install ipfshttpclient
RUN pip3 install gunicorn
RUN pip3 install simplejson


COPY ./webui.py /bc_webui/bc_webui/webui.py
COPY ./ipfs.py /bc_webui/bc_webui/ipfs.py
COPY ./config.ini /bc_webui/config.ini

WORKDIR /bc_webui/bc_webui
ENTRYPOINT [ "gunicorn", "--bind", "0.0.0.0:5007", "webui:app" ]
