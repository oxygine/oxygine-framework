LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LS_CPP=$(subst $(1)/,,$(wildcard $(1)/$(2)/*.cpp))

BOX2D = ../../../Box2D
BOX2DL = ../../../Box2D/Box2D
BOX2D_CPP:= $(call LS_CPP,$(LOCAL_PATH),$(BOX2DL)/Collision) \
            $(call LS_CPP,$(LOCAL_PATH),$(BOX2DL)/Collision/Shapes) \
            $(call LS_CPP,$(LOCAL_PATH),$(BOX2DL)/Common) \
            $(call LS_CPP,$(LOCAL_PATH),$(BOX2DL)/Dynamics) \
            $(call LS_CPP,$(LOCAL_PATH),$(BOX2DL)/Dynamics/Contacts) \
            $(call LS_CPP,$(LOCAL_PATH),$(BOX2DL)/Dynamics/Joints) \
            $(call LS_CPP,$(LOCAL_PATH),$(BOX2DL)/Rope)

LOCAL_MODULE:= box2d_static
LOCAL_SRC_FILES:= $(BOX2D_CPP)
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/$(BOX2D)
LOCAL_C_INCLUDES := $(LOCAL_EXPORT_C_INCLUDES)

include $(BUILD_STATIC_LIBRARY)



include $(CLEAR_VARS)

LOCAL_MODULE := main

#SDK_ROOT points to folder with SDL and oxygine-framework
LOCAL_SRC_FILES := ../../../../../..//SDL/src/main/android/SDL_android_main.c

LOCAL_SRC_FILES += ../../../src/Box2DDebugDraw.cpp ../../../src/main.cpp ../../../src/example.cpp

LOCAL_STATIC_LIBRARIES := oxygine-framework_static box2d_static
LOCAL_SHARED_LIBRARIES := SDL2

include $(BUILD_SHARED_LIBRARY)

#import from NDK_MODULE_PATH defined in build.cmd
$(call import-module, oxygine-framework)
