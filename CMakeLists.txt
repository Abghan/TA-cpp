cmake_minimum_required(VERSION 3.0.0)
project(TA)

find_package(PahoMqttCpp REQUIRED)
find_package(jsoncpp REQUIRED)

add_subdirectory(rover)

add_executable(output main.cpp)
target_link_libraries(output
    rover  
    jsoncpp
    paho-mqtt3as 
    paho-mqttpp3
)

