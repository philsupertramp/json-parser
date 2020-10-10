cmake_minimum_required(VERSION 3.18)
project(json_parser)

set(CMAKE_CXX_STANDARD 17)

add_executable(json_parser main.cpp JsonFile.cpp JsonFile.h TestJsonFile.h)