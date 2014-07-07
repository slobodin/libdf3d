cmake_minimum_required(VERSION 2.8)

project(libdf3d)

file(GLOB_RECURSE SOURCE_DF3D "./*.cpp")

include_directories(audio
                    base
                    gui
                    particlesys
                    physics
                    platform
                    render
                    resources
                    scene
                    scripting
                    utils
                    ../third-party/include)

add_definitions(-D_USRDLL)
add_definitions(-DLIBDF3D_EXPORTS)

add_library(libdf3d SHARED ${SOURCE_DF3D})