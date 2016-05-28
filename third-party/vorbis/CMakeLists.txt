cmake_minimum_required(VERSION 3.1)

project(vorbis)

set(VORBIS_HEADERS_ROOT ${PROJECT_SOURCE_DIR}/)

set(vorbis_HEADER_LIST
    ${VORBIS_HEADERS_ROOT}/codec.h
    ${VORBIS_HEADERS_ROOT}/vorbisenc.h
    ${VORBIS_HEADERS_ROOT}/vorbisfile.h
)

set(vorbis_SRC_LIST
    ${PROJECT_SOURCE_DIR}/analysis.c
    ${PROJECT_SOURCE_DIR}/bitrate.c
    ${PROJECT_SOURCE_DIR}/block.c
    ${PROJECT_SOURCE_DIR}/codebook.c
    ${PROJECT_SOURCE_DIR}/envelope.c
    ${PROJECT_SOURCE_DIR}/floor0.c
    ${PROJECT_SOURCE_DIR}/floor1.c
    ${PROJECT_SOURCE_DIR}/info.c
    ${PROJECT_SOURCE_DIR}/lookup.c
    ${PROJECT_SOURCE_DIR}/lpc.c
    ${PROJECT_SOURCE_DIR}/lsp.c
    ${PROJECT_SOURCE_DIR}/mapping0.c
    ${PROJECT_SOURCE_DIR}/mdct.c
    ${PROJECT_SOURCE_DIR}/psy.c
    ${PROJECT_SOURCE_DIR}/registry.c
    ${PROJECT_SOURCE_DIR}/res0.c
    ${PROJECT_SOURCE_DIR}/sharedbook.c
    ${PROJECT_SOURCE_DIR}/smallft.c
    ${PROJECT_SOURCE_DIR}/synthesis.c
    ${PROJECT_SOURCE_DIR}/vorbisenc.c
    ${PROJECT_SOURCE_DIR}/vorbisfile.c
    ${PROJECT_SOURCE_DIR}/window.c
)

include_directories(${PROJECT_SOURCE_DIR}/../)
include_directories(${PROJECT_SOURCE_DIR})

if (CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
endif()

if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)

    set_source_files_properties(${vorbis_SRC_LIST} PROPERTIES COMPILE_FLAGS "/wd\"4456\" /wd\"4457\" /wd\"4458\" /wd\"4244\" /wd\"4305\"")
endif(MSVC)

add_library(vorbis STATIC ${vorbis_SRC_LIST} ${vorbis_HEADER_LIST})
target_link_libraries(vorbis ogg)
