cmake_minimum_required(VERSION 3.16)

project(imu_visualizer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_subdirectory(ArduiPi_OLED)

add_subdirectory(imu_websockets)

add_library(${PROJECT_NAME}_lib SHARED src/visualizer.cpp)
target_include_directories(${PROJECT_NAME}_lib PUBLIC include/ eigen/)

# add_executable(${PROJECT_NAME} src/main.cpp)
# target_link_libraries(${PROJECT_NAME} PUBLIC ${PROJECT_NAME}_lib ArduiPi_lib imu_websockets_lib)