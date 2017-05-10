if(LIBDF3D_CMAKE_LOADED)
    return()
endif()
set(LIBDF3D_CMAKE_LOADED true)

# Possible platforms
# WINDOWS
# ANDROID
# IOS
# TVOS

if (NOT DEFINED DF3D_PLATFORM)
    message(FATAL_ERROR "Please, specify target platform")
else()
    message(STATUS "DF3D_PLATFORM is " ${DF3D_PLATFORM})
endif()

if(NOT CMAKE_BUILD_TYPE)
    message(WARNING "CMAKE_BUILD_TYPE is set to 'Debug' by default")
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif()

message(STATUS "CMAKE_BUILD_TYPE is " ${CMAKE_BUILD_TYPE})

if (${DF3D_PLATFORM} STREQUAL "WINDOWS")
    set(DF3D_WINDOWS true)
    set(DF3D_DESKTOP true)
    add_definitions(-DDF3D_DESKTOP)

    add_definitions(-DWIN32 -D_WINDOWS -DDF3D_WINDOWS)

    message(STATUS "Platform - Windows")
elseif (${DF3D_PLATFORM} STREQUAL "ANDROID")
    set(DF3D_ANDROID true)

    add_definitions(-DDF3D_ANDROID)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wno-inconsistent-missing-override -Wall")

    message(STATUS "Platform - Android")
elseif (${DF3D_PLATFORM} STREQUAL "IOS")
    set(DF3D_IOS true)

    set(CMAKE_OSX_SYSROOT "iphoneos")

    add_definitions(-DDF3D_IOS)
    add_definitions(-DZ_HAVE_UNISTD_H) # Hack for zlib

    set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++14")
    set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -w")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -stdlib=libc++ -Wno-inconsistent-missing-override -Wno-conversion -Wall")

    message(STATUS "Platform - iOS")
elseif (${DF3D_PLATFORM} STREQUAL "TVOS")
    set(DF3D_IOS true)
    set(DF3D_TVOS true)

    set(CMAKE_OSX_SYSROOT "appletvos")

    add_definitions(-DDF3D_IOS)
    add_definitions(-DDF3D_APPLETV)
    add_definitions(-DZ_HAVE_UNISTD_H) # Hack for zlib

    set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++14")
    set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -w")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -stdlib=libc++ -Wno-inconsistent-missing-override -Wno-conversion -Wall")

    message(STATUS "Platform - tvOS")
else()
    message(FATAL_ERROR "Unknown DF3D_PLATFORM.")
endif()

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-D_DEBUG)
endif()

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    add_definitions(-DNDEBUG)
endif()
