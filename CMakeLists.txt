cmake_minimum_required(VERSION 3.16)

project(imu_visualizer VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(imu_websockets)

add_library(${PROJECT_NAME}_lib SHARED src/visualizer.cpp)
target_include_directories(${PROJECT_NAME}_lib PUBLIC include/ eigen/)
