FROM ubuntu:22.04

RUN apt-get update
RUN apt-get install build-essential git-core libi2c-dev i2c-tools lm-sensors -y

COPY . /repo

WORKDIR /repo/ArduiPi_OLED
RUN ls -ltr
RUN make

WORKDIR /repo/ArduiPi_OLED/examples
RUN make

ENTRYPOINT [ "/repo/examples/oled_demo", "--verbose", "--oled", "3"]