FROM mzandtheraspberrypi/imu_websocket_broadcaster:build-2023-09-10 as build

RUN apt-get update
RUN apt-get install build-essential g++ git-core libi2c-dev i2c-tools lm-sensors cmake unzip wget libboost-all-dev  -y

RUN python3 --version

WORKDIR /

COPY CMakeLists.txt /repo/CMakeLists.txt
COPY eigen /repo/eigen
COPY imu_websockets /repo/imu_websockets
COPY include /repo/include
COPY src /repo/src

WORKDIR /repo/
RUN ls -ltr
RUN mkdir build
WORKDIR /repo/build
RUN cmake -DProtobuf_LIBRARIES=/usr/local/lib/libprotobuf.so -Dprotobuf_ABSL_PROVIDER='package' -Dabsl_DIR=/abseil/CMakeProject/install/lib/cmake/absl ..
RUN make

RUN ls -ltr

COPY imu_visualizer_py /repo/imu_visualizer_py/
RUN cp libimu_visualizer_lib.so /repo/imu_visualizer_py/libimu_visualizer_lib.so

FROM ubuntu:22.04

RUN apt-get update
RUN apt-get install python3.10 python3-pip -y
RUN python3 --version

RUN pip3 install adafruit-circuitpython-ssd1306 numpy matplotlib

RUN apt-get install python3-pil python3-rpi.gpio -y # python3-dev

COPY --from=build /repo/build/imu_websockets/libimu_websockets_lib.so /usr/local/lib/
COPY --from=build /usr/local/lib/libprotobuf.so.24.3.0 /usr/local/lib/
COPY --from=build /repo/imu_visualizer_py/__init__.py /repo/imu_visualizer_py/__init__.py
COPY --from=build /repo/imu_visualizer_py/util.py /repo/imu_visualizer_py/util.py
COPY --from=build /repo/imu_visualizer_py/main.py /repo/imu_visualizer_py/main.py
COPY --from=build /repo/imu_visualizer_py/arial.ttf /repo/imu_visualizer_py/arial.ttf
COPY --from=build /repo/imu_visualizer_py/libimu_visualizer_lib.so /repo/imu_visualizer_py/libimu_visualizer_lib.so
ENV LD_LIBRARY_PATH=/usr/local/lib

STOPSIGNAL SIGINT
RUN chmod +x /repo/imu_visualizer_py/main.py

WORKDIR /repo/imu_visualizer_py
ENTRYPOINT [ "/repo/imu_visualizer_py/main.py"]
