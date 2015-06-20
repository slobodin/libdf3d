cmake_minimum_required(VERSION 3.1)

project(android_native_app)

include_directories(${ANDROID_NDK}/sources/android/native_app_glue)

add_library(android_native_app STATIC ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)
