LOCAL_PATH := $(call my-dir)
OXYGINE_LIBS := oxygine/third_party/android/libraries/$(TARGET_ARCH_ABI)

include $(CLEAR_VARS)
LOCAL_MODULE := ox_libpng_static
LOCAL_MODULE_FILENAME := png
LOCAL_SRC_FILES := $(OXYGINE_LIBS)/libpng.a
include $(PREBUILT_STATIC_LIBRARY) 

include $(CLEAR_VARS)
LOCAL_MODULE := ox_libjpeg_static
LOCAL_MODULE_FILENAME := jpeg
LOCAL_SRC_FILES := $(OXYGINE_LIBS)/libjpeg.a
include $(PREBUILT_STATIC_LIBRARY) 


include $(CLEAR_VARS)

LOCAL_MODULE    := oxygine-framework_static
LOCAL_MODULE_FILENAME := liboxygine-framework
LOCAL_CPP_EXTENSION := .cpp

OXYGINE_SRC := $(LOCAL_PATH)/oxygine/src

LOCAL_SRC_FILES := \
				$(subst $(LOCAL_PATH)/,, \
				$(wildcard $(OXYGINE_SRC)/closure/*.cpp) \
				$(wildcard $(OXYGINE_SRC)/core/*.cpp) \
				$(wildcard $(OXYGINE_SRC)/math/*.cpp) \
				$(wildcard $(OXYGINE_SRC)/pugixml/*.cpp) \
				$(wildcard $(OXYGINE_SRC)/res/*.cpp) \
				$(wildcard $(OXYGINE_SRC)/text_utils/*.cpp) \
				$(wildcard $(OXYGINE_SRC)/dev_tools/*.cpp) \
				$(wildcard $(OXYGINE_SRC)/utils/*.cpp) \
				$(wildcard $(OXYGINE_SRC)/winnie_alloc/*.cpp) \
				$(wildcard $(OXYGINE_SRC)/minizip/*.c) \
				$(wildcard $(OXYGINE_SRC)/*.cpp) \
				)

OXYGINE_SRC := oxygine/src

LOCAL_SRC_FILES += \
				oxygine/greenlets/src/greenlet.c \

LOCAL_SRC_FILES += \
				$(OXYGINE_SRC)/core/gl/NativeTextureGLES.cpp \
				$(OXYGINE_SRC)/core/gl/oxgl.cpp \
				$(OXYGINE_SRC)/core/gl/VertexDeclarationGL.cpp \
				$(OXYGINE_SRC)/core/gl/ShaderProgramGL.cpp \
				$(OXYGINE_SRC)/core/gl/VideoDriverGL.cpp \
				$(OXYGINE_SRC)/core/gl/VideoDriverGLES20.cpp \

SDL_PATH := $(LOCAL_PATH)/../SDL/include/
THIRD_PARTY := $(LOCAL_PATH)/oxygine/third_party/android/

OXYGINE_SRC := $(LOCAL_PATH)/oxygine/src
LOCAL_C_INCLUDES := $(OXYGINE_SRC)/ \
					$(OXYGINE_SRC)/../greenlets/src/ \
					$(SDL_PATH) \
					$(THIRD_PARTY)/libjpeg/ \
					$(THIRD_PARTY)/libpng/

LOCAL_C_FLAGS := -DUSE_FILE32API
LOCAL_EXPORT_C_FLAGS := $(LOCAL_C_FLAGS)
LOCAL_EXPORT_LDLIBS += -lGLESv2 -llog -lz
LOCAL_EXPORT_C_INCLUDES += $(LOCAL_C_INCLUDES)
LOCAL_WHOLE_STATIC_LIBRARIES := ox_libpng_static ox_libjpeg_static
					
include $(BUILD_STATIC_LIBRARY)

$(call import-add-path, $(LOCAL_PATH)/../)
$(call import-module, SDL)