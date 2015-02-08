cmake_minimum_required(VERSION 2.8)

project(lua)

set(LUA_HEADERS_ROOT ${PROJECT_SOURCE_DIR}/../../include/lua)

set(lua_HEADER_LIST
    ${LUA_HEADERS_ROOT}/lapi.h
    ${LUA_HEADERS_ROOT}/lauxlib.h
    ${LUA_HEADERS_ROOT}/lcode.h
    ${LUA_HEADERS_ROOT}/lctype.h
    ${LUA_HEADERS_ROOT}/ldebug.h
    ${LUA_HEADERS_ROOT}/ldo.h
    ${LUA_HEADERS_ROOT}/lfunc.h
    ${LUA_HEADERS_ROOT}/lgc.h
    ${LUA_HEADERS_ROOT}/llex.h
    ${LUA_HEADERS_ROOT}/llimits.h
    ${LUA_HEADERS_ROOT}/lmem.h
    ${LUA_HEADERS_ROOT}/lobject.h
    ${LUA_HEADERS_ROOT}/lopcodes.h
    ${LUA_HEADERS_ROOT}/lparser.h
    ${LUA_HEADERS_ROOT}/lprefix.h
    ${LUA_HEADERS_ROOT}/lstate.h
    ${LUA_HEADERS_ROOT}/lstring.h
    ${LUA_HEADERS_ROOT}/ltable.h
    ${LUA_HEADERS_ROOT}/ltm.h
    ${LUA_HEADERS_ROOT}/lua.h
    ${LUA_HEADERS_ROOT}/lua.hpp
    ${LUA_HEADERS_ROOT}/luaconf.h
    ${LUA_HEADERS_ROOT}/lualib.h
    ${LUA_HEADERS_ROOT}/lundump.h
    ${LUA_HEADERS_ROOT}/lvm.h
    ${LUA_HEADERS_ROOT}/lzio.h
)

set(lua_SRC_LIST
    ${PROJECT_SOURCE_DIR}/lapi.c
    ${PROJECT_SOURCE_DIR}/lauxlib.c
    ${PROJECT_SOURCE_DIR}/lbaselib.c
    ${PROJECT_SOURCE_DIR}/lbitlib.c
    ${PROJECT_SOURCE_DIR}/lcode.c
    ${PROJECT_SOURCE_DIR}/lcorolib.c
    ${PROJECT_SOURCE_DIR}/lctype.c
    ${PROJECT_SOURCE_DIR}/ldblib.c
    ${PROJECT_SOURCE_DIR}/ldebug.c
    ${PROJECT_SOURCE_DIR}/ldo.c
    ${PROJECT_SOURCE_DIR}/ldump.c
    ${PROJECT_SOURCE_DIR}/lfunc.c
    ${PROJECT_SOURCE_DIR}/lgc.c
    ${PROJECT_SOURCE_DIR}/linit.c
    ${PROJECT_SOURCE_DIR}/liolib.c
    ${PROJECT_SOURCE_DIR}/llex.c
    ${PROJECT_SOURCE_DIR}/lmathlib.c
    ${PROJECT_SOURCE_DIR}/lmem.c
    ${PROJECT_SOURCE_DIR}/loadlib.c
    ${PROJECT_SOURCE_DIR}/lobject.c
    ${PROJECT_SOURCE_DIR}/lopcodes.c
    ${PROJECT_SOURCE_DIR}/loslib.c
    ${PROJECT_SOURCE_DIR}/lparser.c
    ${PROJECT_SOURCE_DIR}/lstate.c
    ${PROJECT_SOURCE_DIR}/lstring.c
    ${PROJECT_SOURCE_DIR}/lstrlib.c
    ${PROJECT_SOURCE_DIR}/ltable.c
    ${PROJECT_SOURCE_DIR}/ltablib.c
    ${PROJECT_SOURCE_DIR}/ltm.c
    ${PROJECT_SOURCE_DIR}/lundump.c
    ${PROJECT_SOURCE_DIR}/lutf8lib.c
    ${PROJECT_SOURCE_DIR}/lvm.c
    ${PROJECT_SOURCE_DIR}/lzio.c
)

include_directories(${PROJECT_SOURCE_DIR}/../../include/lua)

add_definitions(-DLUA_BUILD_AS_DLL)

if (WINDOWS)
    add_definitions(-DWIN32 -DDF3D_WINDOWS)
endif()

if (WINDOWS_PHONE)
    add_definitions(-DDF3D_WINDOWS_PHONE)
endif()

add_library(lua SHARED ${lua_SRC_LIST} ${lua_HEADER_LIST})