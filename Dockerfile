FROM ubuntu:22.04

RUN apt-get update
RUN apt-get install build-essential git-core libi2c-dev i2c-tools lm-sensors cmake -y

COPY . /repo

WORKDIR /repo/ArduiPi_OLED
RUN ls -ltr
RUN mkdir build
WORKDIR /repo/ArduiPi_OLED/build
RUN cmake ..
RUN make

RUN ls -ltr

ENTRYPOINT [ "/repo/ArduiPi_OLED/oled_demo", "--verbose", "--oled", "3"]