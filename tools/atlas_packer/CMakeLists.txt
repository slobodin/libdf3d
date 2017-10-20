cmake_minimum_required(VERSION 3.1)

project(atlas_packer)

set(DF3D_ROOT ${PROJECT_SOURCE_DIR}/../../)

include_directories(
    ${DF3D_ROOT}/
    ${DF3D_ROOT}/third-party
    ${DF3D_ROOT}/third-party/bullet/src
    ${DF3D_ROOT}/third-party/spark/include
    ${DF3D_ROOT}/third-party/sqrat
    ${DF3D_ROOT}/third-party/squirrel/include
)

set(atlas_packer_SRC_LIST
    ${PROJECT_SOURCE_DIR}/atlas_packer.cpp

    ${DF3D_ROOT}/third-party/RectangleBinPack/MaxRectsBinPack.h
    ${DF3D_ROOT}/third-party/RectangleBinPack/MaxRectsBinPack.cpp
    ${DF3D_ROOT}/third-party/RectangleBinPack/Rect.h
    ${DF3D_ROOT}/third-party/RectangleBinPack/Rect.cpp
)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd\"4251\" /wd\"4457\" /wd\"4458\" /wd\"4138\"")
    add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS)    #  -DGLEW_STATIC
endif()

add_executable(atlas_packer ${atlas_packer_SRC_LIST})

target_link_libraries(atlas_packer libdf3d)
