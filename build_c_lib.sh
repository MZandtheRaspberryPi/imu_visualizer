#!/bin/bash

set -eux o pipefail

cd build
cmake ..
make
cp ./libimu_visualizer_lib.so ../imu_visualizer_py/
cd ../imu_visualizer_py
