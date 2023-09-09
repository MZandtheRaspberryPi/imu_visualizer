FROM ubuntu:22.04

RUN apt-get update
RUN apt-get install build-essential g++ git-core libi2c-dev i2c-tools lm-sensors cmake unzip wget protobuf-compiler libboost-all-dev  -y

RUN python3 --version
RUN apt-get install python3-pip -y


RUN pip3 install adafruit-circuitpython-ssd1306

RUN apt-get install python3-pil -y

RUN pip3 install numpy

WORKDIR /
COPY . /repo

RUN chmod +x /repo/imu_visualizer_py/main.py

WORKDIR /repo/
RUN ls -ltr
RUN mkdir build
WORKDIR /repo/build
RUN cmake ..
RUN make

RUN ls -ltr

# ENTRYPOINT [ "/repo/ArduiPi_OLED/build/oled_demo", "--verbose", "--oled", "3"]
ENTRYPOINT [ "/repo/imu_visualizer_py/main.py"]
