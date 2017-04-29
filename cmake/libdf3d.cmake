if(LIBDF3D_CMAKE_LOADED)
    return()
endif()
set(LIBDF3D_CMAKE_LOADED true)

# include(CheckCXXCompilerFlag)

if(NOT CMAKE_BUILD_TYPE)
    message(WARNING "CMAKE_BUILD_TYPE default set to DEBUG")
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif()

message(STATUS "CMAKE_BUILD_TYPE is " ${CMAKE_BUILD_TYPE})

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(DF3D_WINDOWS true)

    message(STATUS "Platform - Windows")
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(DF3D_LINUX true)

    message(STATUS "Platform - Linux")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wno-inconsistent-missing-override -Wall")
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Android")
    set(DF3D_ANDROID true)

    message(STATUS "Platform - Android")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wno-inconsistent-missing-override -Wall")
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    if (IOS)
        message(STATUS "Platform - iOS")
        set(DF3D_IOS true)
        if (APPLETV_OS)
            message(STATUS "AppleTV")
            set(DF3D_TVOS true)
        endif()
    else()
        message(FATAL_ERROR "Unknown Apple system.")
    endif()
    set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++14")
    set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -w")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -stdlib=libc++ -Wno-inconsistent-missing-override -Wno-conversion -Wall")

    # CHECK_CXX_COMPILER_FLAG("-x objective-c++" HAVE_OBJC)
    # if(HAVE_OBJC)
    #     set(CMAKE_REQUIRED_FLAGS "-x none")
    #     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -x objective-c++")
    #     set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -x none -ObjC")
    # endif()
else()
    message(FATAL_ERROR "Unknown system.")
endif()

if (DF3D_WINDOWS OR DF3D_LINUX)
    set(DF3D_DESKTOP true)
endif()

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-D_DEBUG)
endif()

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    add_definitions(-DNDEBUG)
endif()

if (DF3D_WINDOWS)
    add_definitions(-DWIN32 -D_WINDOWS -DDF3D_WINDOWS)
    set(DF3D_PLATFORM "Windows")
endif()

if (DF3D_ANDROID)
    add_definitions(-DDF3D_ANDROID)
    set(DF3D_PLATFORM "Android")
endif()

if (DF3D_LINUX)
    add_definitions(-DDF3D_LINUX)
    set(DF3D_PLATFORM "Linux")
endif()

if (DF3D_DESKTOP)
    add_definitions(-DDF3D_DESKTOP)
endif()

if (DF3D_IOS)
    add_definitions(-DDF3D_IOS)
    add_definitions(-DZ_HAVE_UNISTD_H) # Hack for zlib
    set(DF3D_PLATFORM "iOS")
endif()

if (DF3D_TVOS)
    add_definitions(-DDF3D_APPLETV)
    add_definitions(-DZ_HAVE_UNISTD_H) # Hack for zlib
    set(DF3D_PLATFORM "tvOS")
endif()

