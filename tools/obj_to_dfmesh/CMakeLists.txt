cmake_minimum_required(VERSION 3.1)

project(obj_to_dfmesh)

include_directories(
    ${DF3D_ROOT}/
    ${DF3D_ROOT}/third-party
    ${DF3D_ROOT}/third-party/bullet/src
    ${DF3D_ROOT}/third-party/spark/include
    ${DF3D_ROOT}/third-party/sqrat
    ${DF3D_ROOT}/third-party/squirrel/include
)

set(obj_to_dfmesh_SRC_LIST
    ${PROJECT_SOURCE_DIR}/main_obj_to_dfmesh.cpp
)

add_definitions(-DBULLET_FORCE_NO_ALIGNMENT)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd\"4251\" /wd\"4457\" /wd\"4458\" /wd\"4138\"")
    add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS)    #  -DGLEW_STATIC
endif()

if (DF3D_BUILD_SHARED_LIB)
    add_definitions(-DJSON_DLL -DDF3D_SHARED_LIBRARY)
endif()

add_executable(obj_to_dfmesh ${obj_to_dfmesh_SRC_LIST})

target_link_libraries(obj_to_dfmesh libdf3d)
