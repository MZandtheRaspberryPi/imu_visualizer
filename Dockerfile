FROM ubuntu:22.04

RUN apt-get update
RUN apt-get install build-essential g++ git-core libi2c-dev i2c-tools lm-sensors cmake unzip wget protobuf-compiler libboost-all-dev  -y


WORKDIR /
COPY . /repo

WORKDIR /repo/
RUN ls -ltr
RUN mkdir build
WORKDIR /repo/build
RUN cmake ..
RUN make

RUN ls -ltr

ENTRYPOINT [ "/repo/ArduiPi_OLED/build/oled_demo", "--verbose", "--oled", "3"]
