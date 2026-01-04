cmake_minimum_required (VERSION 3.31)

project(rwr LANGUAGES CXX C)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SRCS
    src/Main.cpp
    
    src/Optional.h
)

add_executable(${PROJECT_NAME} ${SRCS})