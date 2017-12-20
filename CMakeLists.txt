cmake_minimum_required(VERSION 3.9)
project(SocketPthreadPractise C)

set(CMAKE_C_STANDARD 90)
set(CMAKE_C_FLAGS -lpthread)

add_executable(SocketPthreadPractise main.c main.h client.c client.h)
target_link_libraries(SocketPthreadPractise pthread)