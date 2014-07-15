LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE:= libogg
LOCAL_SRC_FILES:= ../../../lib/android/$(TARGET_ARCH_ABI)/libogg.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE := libvorbis
LOCAL_CFLAGS += -I$(LOCAL_PATH)/../../../include -I$(LOCAL_PATH)/../ -ffast-math -fsigned-char
ifeq ($(TARGET_ARCH),arm)
	LOCAL_CFLAGS += -march=armv6 -marm -mfloat-abi=softfp -mfpu=vfp
endif
LOCAL_SHARED_LIBRARIES := libogg

LOCAL_SRC_FILES := \
	../mdct.c		\
	../smallft.c	\
	../block.c		\
	../envelope.c	\
	../window.c	\
	../lsp.c		\
	../lpc.c		\
	../analysis.c	\
	../synthesis.c	\
	../psy.c		\
	../info.c		\
	../floor1.c	\
	../floor0.c	\
	../res0.c		\
	../mapping0.c	\
	../registry.c	\
	../codebook.c	\
	../sharedbook.c	\
	../lookup.c	\
	../bitrate.c	\
	../vorbisfile.c	\
	../vorbisenc.c

include $(BUILD_SHARED_LIBRARY)