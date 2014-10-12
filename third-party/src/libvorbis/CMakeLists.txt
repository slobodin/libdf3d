cmake_minimum_required(VERSION 2.8)

project(libvorbis)

set(LIBVORBIS_HEADERS_ROOT ${PROJECT_SOURCE_DIR}/../../include/vorbis)

set(libvorbis_HEADER_LIST
    ${LIBVORBIS_HEADERS_ROOT}/codec.h
    ${LIBVORBIS_HEADERS_ROOT}/vorbisenc.h
    ${LIBVORBIS_HEADERS_ROOT}/vorbisfile.h
)

set(libvorbis_SRC_LIST
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

include_directories(${PROJECT_SOURCE_DIR}/../../include)
include_directories(${PROJECT_SOURCE_DIR}/../../src/libvorbis)

if(MSVC)
    add_definitions(-DWIN32 -D_CRT_SECURE_NO_WARNINGS)

    set_source_files_properties(${libvorbis_SRC_LIST} PROPERTIES COMPILE_FLAGS "/wd\"4456\" /wd\"4457\" /wd\"4458\" /wd\"4244\" /wd\"4305\"")
endif(MSVC)

add_library(libvorbis STATIC ${libvorbis_SRC_LIST} ${libvorbis_HEADER_LIST})
