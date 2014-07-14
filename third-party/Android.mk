LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE:= librocket
LOCAL_SRC_FILES:= lib/android/$(TARGET_ARCH_ABI)/librocket.so
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include/
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE:= libpyrocketcore
LOCAL_SRC_FILES:= lib/android/$(TARGET_ARCH_ABI)/libpyrocketcore.so
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include/
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE:= libfreetype
LOCAL_SRC_FILES:= lib/android/$(TARGET_ARCH_ABI)/libfreetype2.so
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include/
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE:= libopenal
LOCAL_SRC_FILES:= lib/android/$(TARGET_ARCH_ABI)/libopenal.so
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include/
include $(PREBUILT_SHARED_LIBRARY)

# include $(CLEAR_VARS)
# LOCAL_MODULE:= libbullet
# LOCAL_SRC_FILES:= lib/android/$(TARGET_ARCH_ABI)/libbullet.so
# LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include/
# include $(PREBUILT_SHARED_LIBRARY)

# include $(CLEAR_VARS)
# LOCAL_MODULE:= libspark
# LOCAL_SRC_FILES:= lib/android/$(TARGET_ARCH_ABI)/libspark.so
# LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include/
# include $(PREBUILT_SHARED_LIBRARY)

# include $(CLEAR_VARS)
# LOCAL_MODULE:= libogg
# LOCAL_SRC_FILES:= lib/android/$(TARGET_ARCH_ABI)/libogg.so
# LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include/
# include $(PREBUILT_SHARED_LIBRARY)

# include $(CLEAR_VARS)
# LOCAL_MODULE:= libvorbis
# LOCAL_SRC_FILES:= lib/android/$(TARGET_ARCH_ABI)/libvorbis.so
# LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include/
# include $(PREBUILT_SHARED_LIBRARY)