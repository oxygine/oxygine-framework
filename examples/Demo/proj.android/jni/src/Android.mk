LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := main

SDK_ROOT = /../../../../
OXYGINE_ROOT := $(SDK_ROOT)/oxygine-framework/
SDL_PATH := $(SDK_ROOT)/SDL

EXAMPLE_SRC := $(SDK_ROOT)/oxygine-framework/examples/demo/src/
OXYGINE := $(OXYGINE_ROOT)/oxygine/src/

# Add your application source files here...
LOCAL_SRC_FILES :=  $(SDL_PATH)/src/main/android/SDL_android_main.c \
					$(EXAMPLE_SRC)/entry_point.cpp \
					$(EXAMPLE_SRC)/example.cpp \
					$(EXAMPLE_SRC)/test.cpp
#					../SDL/test/testgles.c \
#					../SDL/test/common.c


OXYGINE_LIBS := $(OXYGINE_ROOT)/oxygine/third_party/android/libraries/$(TARGET_ARCH_ABI)

# LOCAL_LDLIBS := -L$(call host-path, $(OXYGINE_LIBS)) \
#                 -ljpeg
				

LOCAL_STATIC_LIBRARIES := oxygine-framework_static
LOCAL_SHARED_LIBRARIES := SDL2

include $(BUILD_SHARED_LIBRARY)

# $(call import-module, oxygine-frame) 
$(call import-add-path, $(LOCAL_PATH)/../../../../../../)
$(call import-module, oxygine-framework) 


