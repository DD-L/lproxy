# lproxy-dev
# docker build -t="deel/lproxy-dev:lss-test"
# docker run -it -p 8087-8088:8087-8088 deel/lproxy-dev:lss-test /bin/bash
# Author: Deel

FROM docker.io/deel/ubuntu-mydev:14.04.4

MAINTAINER deel deel@d-l.top

#EXPONSE 8087
#EXPONSE 8088

COPY ../../* /opt/lproxy
WORKDIR /opt

RUN WORKDIR=$PWD \
    && set -x \
    && mkdir -p lproxy/contrib/Python-2.7.11/build \
    && cd lproxy/contrib/Python-2.7.11/build \
    && wget https://www.python.org/ftp/python/2.7.11/Python-2.7.11.tar.xz \
    && tar xvfJ Python-2.7.11.tar.xz > /dev/null \
    && cd Python-2.7.11 \
    && ./configure --prefix=$WORKDIR/lproxy/contrib/Python-2.7.11/ > /dev/null \
    && make >/dev/null 2>&1 \
    && make install > /dev/null \
    && cd $WORKDIR \
    && rm -rf $WORKDIR/lproxy/contrib/Python-2.7.11/build/ 

WORKDIR /opt/lproxy

CMD make init
