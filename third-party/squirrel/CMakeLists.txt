cmake_minimum_required(VERSION 2.8)

project(squirrel)

if (NOT DEFINED DF3D_PLATFORM)
    message(FATAL_ERROR "Please, include libdf3d.cmake")
endif()

set(squirrel_HEADER_LIST
    ${PROJECT_SOURCE_DIR}/include/sqstdaux.h
    ${PROJECT_SOURCE_DIR}/include/sqstdblob.h
    ${PROJECT_SOURCE_DIR}/include/sqstdio.h
    ${PROJECT_SOURCE_DIR}/include/sqstdmath.h
    ${PROJECT_SOURCE_DIR}/include/sqstdstring.h
    ${PROJECT_SOURCE_DIR}/include/squirrel.h
    ${PROJECT_SOURCE_DIR}/include/sqconfig.h

    ${PROJECT_SOURCE_DIR}/squirrel/sqarray.h
    ${PROJECT_SOURCE_DIR}/squirrel/sqclass.h
    ${PROJECT_SOURCE_DIR}/squirrel/sqclosure.h
    ${PROJECT_SOURCE_DIR}/squirrel/sqcompiler.h
    ${PROJECT_SOURCE_DIR}/squirrel/sqfuncproto.h
    ${PROJECT_SOURCE_DIR}/squirrel/sqfuncstate.h
    ${PROJECT_SOURCE_DIR}/squirrel/sqlexer.h
    ${PROJECT_SOURCE_DIR}/squirrel/sqobject.h
    ${PROJECT_SOURCE_DIR}/squirrel/sqopcodes.h
    ${PROJECT_SOURCE_DIR}/squirrel/sqpcheader.h
    ${PROJECT_SOURCE_DIR}/squirrel/sqstate.h
    ${PROJECT_SOURCE_DIR}/squirrel/sqstring.h
    ${PROJECT_SOURCE_DIR}/squirrel/sqtable.h
    ${PROJECT_SOURCE_DIR}/squirrel/squserdata.h
    ${PROJECT_SOURCE_DIR}/squirrel/squtils.h
    ${PROJECT_SOURCE_DIR}/squirrel/sqvm.h

    ${PROJECT_SOURCE_DIR}/sqstdlib/sqstdblobimpl.h
    ${PROJECT_SOURCE_DIR}/sqstdlib/sqstdstream.h
)

set(squirrel_SRC_LIST
    ${PROJECT_SOURCE_DIR}/squirrel/sqapi.cpp
    ${PROJECT_SOURCE_DIR}/squirrel/sqbaselib.cpp
    ${PROJECT_SOURCE_DIR}/squirrel/sqclass.cpp
    ${PROJECT_SOURCE_DIR}/squirrel/sqcompiler.cpp
    ${PROJECT_SOURCE_DIR}/squirrel/sqdebug.cpp
    ${PROJECT_SOURCE_DIR}/squirrel/sqfuncstate.cpp
    ${PROJECT_SOURCE_DIR}/squirrel/sqlexer.cpp
    ${PROJECT_SOURCE_DIR}/squirrel/sqmem.cpp
    ${PROJECT_SOURCE_DIR}/squirrel/sqobject.cpp
    ${PROJECT_SOURCE_DIR}/squirrel/sqstate.cpp
    ${PROJECT_SOURCE_DIR}/squirrel/sqtable.cpp
    ${PROJECT_SOURCE_DIR}/squirrel/sqvm.cpp

    ${PROJECT_SOURCE_DIR}/sqstdlib/sqstdaux.cpp
    ${PROJECT_SOURCE_DIR}/sqstdlib/sqstdblob.cpp
    ${PROJECT_SOURCE_DIR}/sqstdlib/sqstdio.cpp
    ${PROJECT_SOURCE_DIR}/sqstdlib/sqstdmath.cpp
    ${PROJECT_SOURCE_DIR}/sqstdlib/sqstdrex.cpp
    ${PROJECT_SOURCE_DIR}/sqstdlib/sqstdstream.cpp
    ${PROJECT_SOURCE_DIR}/sqstdlib/sqstdstring.cpp
)

include_directories(${PROJECT_SOURCE_DIR}/include)

if (CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-exceptions -fno-rtti -Wall -fno-strict-aliasing")
endif()

if (WINDOWS)
    add_definitions(-DWIN32 -DDF3D_WINDOWS)
endif()

if (WINDOWS_PHONE)
    add_definitions(-DDF3D_WINDOWS_PHONE -D_CRT_NON_CONFORMING_SWPRINTFS -D_CRT_SECURE_NO_WARNINGS)
endif()

add_library(squirrel STATIC ${squirrel_SRC_LIST} ${squirrel_HEADER_LIST})
