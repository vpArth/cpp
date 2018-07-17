cmake_minimum_required(VERSION 2.4)
project(dev)

set(CMAKE_CXX_STANDARD 17)

add_executable(dev main.cpp src/ee/EventEmitter.h src/ee/EventEmitter.cpp)