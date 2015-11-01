cmake_minimum_required(VERSION 3.3)
project(Keystore)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp getch.h)
add_executable(Keystore ${SOURCE_FILES})

target_link_libraries(Keystore ssl) #link the ssl library
target_link_libraries(Keystore crypto) #link the crypto library
