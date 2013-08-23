LOCAL_PATH := $(call my-dir)/../../../src
include $(CLEAR_VARS)

LOCAL_CFLAGS := -DOXYGINE_SDL=1
LOCAL_MODULE    := oxygine
LOCAL_CPP_EXTENSION := .cpp

LOCAL_SRC_FILES := \
				$(subst $(LOCAL_PATH)/,, \
				$(wildcard $(LOCAL_PATH)/closure/*.cpp) \
				$(wildcard $(LOCAL_PATH)/core/*.cpp) \
				$(wildcard $(LOCAL_PATH)/core/gl/*.cpp) \
				$(wildcard $(LOCAL_PATH)/math/*.cpp) \
				$(wildcard $(LOCAL_PATH)/pugixml/*.cpp) \
				$(wildcard $(LOCAL_PATH)/res/*.cpp) \
				$(wildcard $(LOCAL_PATH)/text_utils/*.cpp) \
				$(wildcard $(LOCAL_PATH)/dev_tools/*.cpp) \
				$(wildcard $(LOCAL_PATH)/utils/*.cpp) \
				$(wildcard $(LOCAL_PATH)/winnie_alloc/*.cpp) \
				$(wildcard $(LOCAL_PATH)/*.cpp) \
				$(wildcard $(LOCAL_PATH)/../greenlets/src*.c) \
				)


SDL_PATH := $(call my-dir)/../../../../../sdl/include/


  
OXYGINE := $(LOCAL_PATH)
OXYGINE_ROOT := $(OXYGINE)/../
THIRD_PARTY := $(OXYGINE_ROOT)/third_party/android/

LOCAL_C_INCLUDES := $(OXYGINE)/ \
					$(OXYGINE)/closure/ \
					$(OXYGINE)/core/ \
					$(OXYGINE)/math/ \
					$(OXYGINE)/pugixml/ \
					$(OXYGINE)/res/ \
					$(OXYGINE)/utils/ \
					$(OXYGINE)/../greenlets/src/ \
					$(SDL_PATH) \
					$(THIRD_PARTY)/libjpeg/ \
					$(THIRD_PARTY)/libpng/
					

					
include $(BUILD_STATIC_LIBRARY)