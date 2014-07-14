include $(CLEAR_VARS)
LOCAL_PATH := $(call my-dir)/../
LOCAL_MODULE := bullet_shared

LOCAL_MODULE_FILENAME := libbullet
LOCAL_CPPFLAGS += -O3
LOCAL_CFLAGS := $(LOCAL_CPPFLAGS)

LOCAL_C_INCLUDES := $(LOCAL_PATH) \
                    $(LOCAL_PATH)/LinearMath \
                    $(LOCAL_PATH)/BulletDynamics \
                    $(LOCAL_PATH)/BulletDynamics/Dynamics \
                    $(LOCAL_PATH)/BulletDynamics/Charecter \
                    $(LOCAL_PATH)/BulletDynamics/ConstraintSolver \
                    $(LOCAL_PATH)/BulletCollision \
                    $(LOCAL_PATH)/BulletCollision/BroadphaseCollision \
                    $(LOCAL_PATH)/BulletCollision/CollisionDispatch \
                    $(LOCAL_PATH)/BulletCollision/CollisionShapes \
                    $(LOCAL_PATH)/BulletCollision/Gimpact \
                    $(LOCAL_PATH)/BulletCollision/NarrowPhaseCollision
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