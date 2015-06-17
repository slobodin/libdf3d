cmake_minimum_required(VERSION 2.8)

project(libogg)

set(LIBOGG_HEADERS_ROOT ${PROJECT_SOURCE_DIR})

set(libogg_HEADER_LIST
    ${LIBOGG_HEADERS_ROOT}/config_types.h
    ${LIBOGG_HEADERS_ROOT}/ogg.h
    ${LIBOGG_HEADERS_ROOT}/os_types.h
)

set(libogg_SRC_LIST
    ${PROJECT_SOURCE_DIR}/bitwise.c
    ${PROJECT_SOURCE_DIR}/framing.c
)

include_directories(${PROJECT_SOURCE_DIR}/../)

if (CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
endif()

if(MSVC)
    add_definitions(-DWIN32)

    set_source_files_properties(${libogg_SRC_LIST} PROPERTIES COMPILE_FLAGS "/wd\"4456\" /wd\"4457\" /wd\"4458\"")
endif(MSVC)

add_library(libogg STATIC ${libogg_SRC_LIST} ${libogg_HEADER_LIST})
