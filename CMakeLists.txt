cmake_minimum_required(VERSION 3.16)
project(PortfolioGA)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -lcurl -lsqlite3")

include_directories(json)

add_executable(PortfolioGA
        json/json-forwards.h
        json/json.h
        jsoncpp.cpp
        main.cpp database.cpp database.h marketdata.cpp marketdata.h)