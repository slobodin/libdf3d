LOCAL_PATH := $(call my-dir)/../

include $(CLEAR_VARS)

LOCAL_MODULE := bullet_shared

LOCAL_MODULE_FILENAME := libbullet
LOCAL_CPPFLAGS += -O3
LOCAL_CFLAGS := $(LOCAL_CPPFLAGS)

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../include/bullet/ \
                    $(LOCAL_PATH)/../../include/bullet/LinearMath \
                    $(LOCAL_PATH)/../../include/bullet/BulletDynamics \
                    $(LOCAL_PATH)/../../include/bullet/BulletDynamics/Dynamics \
                    $(LOCAL_PATH)/../../include/bullet/BulletDynamics/Character \
                    $(LOCAL_PATH)/../../include/bullet/BulletDynamics/ConstraintSolver \
                    $(LOCAL_PATH)/../../include/bullet/BulletCollision \
                    $(LOCAL_PATH)/../../include/bullet/BulletCollision/BroadphaseCollision \
                    $(LOCAL_PATH)/../../include/bullet/BulletCollision/CollisionDispatch \
                    $(LOCAL_PATH)/../../include/bullet/BulletCollision/CollisionShapes \
                    $(LOCAL_PATH)/../../include/bullet/BulletCollision/Gimpact \
                    $(LOCAL_PATH)/../../include/bullet/BulletCollision/NarrowPhaseCollision
LOCAL_SRC_FILES := \
	$(subst $(LOCAL_PATH)/,, \
	$(wildcard $(LOCAL_PATH)/*.c) \
        $(wildcard $(LOCAL_PATH)/LinearMath/*.cpp) \
        $(wildcard $(LOCAL_PATH)/BulletDynamics/*.cpp) \
        $(wildcard $(LOCAL_PATH)/BulletDynamics/Dynamics/*.cpp) \
        $(wildcard $(LOCAL_PATH)/BulletDynamics/ConstraintSolver/*.cpp) \
        $(wildcard $(LOCAL_PATH)/BulletDynamics/Character/*.cpp) \
        $(wildcard $(LOCAL_PATH)/BulletCollision/BroadphaseCollision/*.cpp) \
        $(wildcard $(LOCAL_PATH)/BulletCollision/CollisionDispatch/*.cpp) \
        $(wildcard $(LOCAL_PATH)/BulletCollision/CollisionShapes/*.cpp) \
        $(wildcard $(LOCAL_PATH)/BulletCollision/Gimpact/*.cpp) \
        $(wildcard $(LOCAL_PATH)/BulletCollision/NarrowPhaseCollision/*.cpp))

include $(BUILD_SHARED_LIBRARY)