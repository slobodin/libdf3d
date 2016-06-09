cmake_minimum_required(VERSION 3.1)

project(ogg)

set(OGG_HEADERS_ROOT ${PROJECT_SOURCE_DIR})

set(ogg_HEADER_LIST
    ${OGG_HEADERS_ROOT}/config_types.h
    ${OGG_HEADERS_ROOT}/ogg.h
    ${OGG_HEADERS_ROOT}/os_types.h
)

set(ogg_SRC_LIST
    ${PROJECT_SOURCE_DIR}/framing.c
)

include_directories(${PROJECT_SOURCE_DIR}/../)

if (CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
endif()

if(MSVC)
    add_definitions(-DWIN32)

    set_source_files_properties(${ogg_SRC_LIST} PROPERTIES COMPILE_FLAGS "/wd\"4456\" /wd\"4457\" /wd\"4458\"")
endif(MSVC)

add_library(ogg STATIC ${ogg_SRC_LIST} ${ogg_HEADER_LIST})
