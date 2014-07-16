LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := libdf3d

LOCAL_CPP_EXTENSION := .cpp

LOCAL_SRC_FILES := \
        ../df3d_pch.cpp \
        ../../third-party/src/lib_json/json_reader.cpp \
        ../../third-party/src/lib_json/json_value.cpp \
        ../../third-party/src/lib_json/json_writer.cpp \
        $(subst $(LOCAL_PATH)/,, \
        $(wildcard $(LOCAL_PATH)/../audio/*.cpp) \
        $(wildcard $(LOCAL_PATH)/../base/*.cpp) \
        $(wildcard $(LOCAL_PATH)/../components/*.cpp) \
        $(wildcard $(LOCAL_PATH)/../gui/*.cpp) \
        $(wildcard $(LOCAL_PATH)/../particlesys/*.cpp) \
        $(wildcard $(LOCAL_PATH)/../platform/*.cpp) \
        $(wildcard $(LOCAL_PATH)/../platform/android/*.cpp) \
        $(wildcard $(LOCAL_PATH)/../render/*.cpp) \
        $(wildcard $(LOCAL_PATH)/../resources/*.cpp) \
        $(wildcard $(LOCAL_PATH)/../resources/decoders/*.cpp) \
        $(wildcard $(LOCAL_PATH)/../physics/*.cpp) \
        $(wildcard $(LOCAL_PATH)/../scene/*.cpp) \
        $(wildcard $(LOCAL_PATH)/../scripting/*.cpp) \
        $(wildcard $(LOCAL_PATH)/../scene/bounding_volumes/*.cpp) \
        $(wildcard $(LOCAL_PATH)/../utils/*.cpp))

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../ \
                    $(LOCAL_PATH)/../../third-party/include/ \
                    $(LOCAL_PATH)/../../third-party/include/bullet \
                    $(LOCAL_PATH)/../../third-party/include/spark \
                    $(LOCAL_PATH)/../../third-party/include/freetype

LOCAL_EXPORT_C_INCLUDES := $(LOCAL_C_INCLUDES)

LOCAL_PCH := ../df3d_pch.h
LOCAL_CPPFLAGS := -DNDEBUG -DGL_GLEXT_PROTOTYPES -DBOOST_DISABLE_ASSERTS -O3 -D_USRDLL -DLIBDF3D_EXPORTS -DJSON_DLL_BUILD -DDF3D_LIBRARY -DGLM_FORCE_CXX98
LOCAL_CFLAGS := $(LOCAL_CPPFLAGS)

LOCAL_LDLIBS := -lGLESv2 \
                -llog \
                -lz \
                -landroid

LOCAL_EXPORT_LDLIBS := $(LOCAL_LDLIBS)

LOCAL_STATIC_LIBRARIES := boost_system boost_python freetype2
LOCAL_SHARED_LIBRARIES := SDL2 SDL2_image python librocket libbullet libspark libopenal libogg libvorbis

include $(BUILD_SHARED_LIBRARY)

$(call import-module, boost)
$(call import-module, SDL)
$(call import-module, python)