LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := main

SDK_ROOT = /../../../../../../
OXYGINE_ROOT := $(SDK_ROOT)/oxygine-framework/
SDL_PATH := $(SDK_ROOT)/SDL

$(warning $(LOCAL_PATH))
SRC := ../../../src
LOCAL_SRC_FILES :=  ../../../../../../SDL/src/main/android/SDL_android_main.c \
					$(SRC)/entry_point.cpp \
					$(SRC)/example.cpp \
					$(SRC)/test.cpp


OXYGINE_LIBS := $(OXYGINE_ROOT)/oxygine/third_party/android/libraries/$(TARGET_ARCH_ABI)


LOCAL_STATIC_LIBRARIES := oxygine-framework_static
LOCAL_SHARED_LIBRARIES := SDL2

include $(BUILD_SHARED_LIBRARY)



$(call import-add-path, $(LOCAL_PATH)/../../../../../../)
$(call import-module, oxygine-framework) 


