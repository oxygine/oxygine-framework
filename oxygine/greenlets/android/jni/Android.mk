LOCAL_PATH := $(call my-dir)/../../src/
include $(CLEAR_VARS)
LOCAL_MODULE    := greenlets
LOCAL_SRC_FILES = $(LOCAL_PATH)/greenlet.c					
include $(BUILD_STATIC_LIBRARY)