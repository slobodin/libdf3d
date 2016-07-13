cmake_minimum_required(VERSION 3.1)

project(glsl_preprocess)

include_directories(
    ${DF3D_ROOT}/
    ${DF3D_ROOT}/third-party
    ${DF3D_ROOT}/third-party/bullet/src
    ${DF3D_ROOT}/third-party/spark/include
    ${DF3D_ROOT}/third-party/sqrat
    ${DF3D_ROOT}/third-party/squirrel/include
)

set(glsl_preprocess_SRC_LIST
    ${PROJECT_SOURCE_DIR}/main_glsl_preprocess.cpp
)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd\"4251\" /wd\"4457\" /wd\"4458\" /wd\"4138\"")
    add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS)    #  -DGLEW_STATIC
endif()

if (DF3D_BUILD_SHARED_LIB)
    add_definitions(-DJSON_DLL -DDF3D_SHARED_LIBRARY)
endif()

add_executable(glsl_preprocess ${glsl_preprocess_SRC_LIST})

target_link_libraries(glsl_preprocess libdf3d)
