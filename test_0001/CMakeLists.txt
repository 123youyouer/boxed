cmake_minimum_required(VERSION 3.12)
project(test_0001)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")

set(BOOST_ROOT "/home/anyone/opt/boost")


include_directories(${BOOST_ROOT}/include)

add_executable(test_0001 main.cpp test_hana.hpp)