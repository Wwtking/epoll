cmake_minimum_required(VERSION 2.8)
project(test)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O3 -g -std=c++11 -Wall -Wno-deprecated -Wno-unused-function -Wno-builtin-macro-redefined") 

add_executable(pthread_test test.cpp)
target_link_libraries(pthread_test pthread)  # 为目标可执行文件添加需要链接的共享库(动态库)

add_executable(client_test client.cpp)
add_executable(epoll_test tcpepoll.cpp)
