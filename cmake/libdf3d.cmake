if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif()

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    if (${CMAKE_SYSTEM_NAME} STREQUAL "WindowsPhone")
        set(WINDOWS_PHONE true)
        message(STATUS "Platform - WindowsPhone")
    else()
        set(WINDOWS true)
        message(STATUS "Platform - Windows")
    endif()
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(LINUX true)

    message(STATUS "Platform - Linux")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Android")
    message(STATUS "Platform - Android")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    if (IOS)

    else()
        set(MACOSX true)
        message(STATUS "Platform - Mac OS")

        set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++14")
        set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -stdlib=libc++")
    endif()
else()
    message(FATAL_ERROR "Unknown system.")
endif()

if (WINDOWS OR LINUX OR MACOSX)
    set(DF3D_DESKTOP true)
endif()

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif()

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-D_DEBUG)
endif()

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    add_definitions(-DNDEBUG)
endif()

if (WINDOWS)
    add_definitions(-DWIN32 -D_WINDOWS -DDF3D_WINDOWS)
endif()

if (ANDROID)
    add_definitions(-DDF3D_ANDROID)
endif()

if (WINDOWS_PHONE)
    add_definitions(-DDF3D_WINDOWS_PHONE)
endif()

if (LINUX)
    add_definitions(-DDF3D_LINUX)
endif()

if (MACOSX)
    add_definitions(-DDF3D_MACOSX)
endif()

if (DF3D_DESKTOP)
    add_definitions(-DDF3D_DESKTOP)
endif()
