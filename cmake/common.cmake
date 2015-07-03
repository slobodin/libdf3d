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
    add_definitions(-DWIN32 D_WINDOWS -DDF3D_WINDOWS)
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
