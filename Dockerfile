FROM ubuntu:14.04

ADD . /build
WORKDIR /build
RUN qmake
RUN qmake
