cmake_minimum_required(VERSION 2.8.12)

project(pugixml)

set(HEADERS src/pugixml.hpp src/pugiconfig.hpp)
set(SOURCES src/pugixml.cpp)

add_library(pugixml STATIC ${HEADERS} ${SOURCES})
