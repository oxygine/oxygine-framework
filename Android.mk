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

SRC := $(LOCAL_PATH)/oxygine/src

LOCAL_SRC_FILES := \
				$(subst $(LOCAL_PATH)/,, \
				$(wildcard $(SRC)/closure/*.cpp) \
				$(wildcard $(SRC)/core/*.cpp) \
				$(wildcard $(SRC)/math/*.cpp) \
				$(wildcard $(SRC)/pugixml/*.cpp) \
				$(wildcard $(SRC)/json/*.cpp) \
				$(wildcard $(SRC)/res/*.cpp) \
				$(wildcard $(SRC)/text_utils/*.cpp) \
				$(wildcard $(SRC)/dev_tools/*.cpp) \
				$(wildcard $(SRC)/utils/*.cpp) \
				$(wildcard $(SRC)/utils/*.c) \
				$(wildcard $(SRC)/winnie_alloc/*.cpp) \
				$(wildcard $(SRC)/minizip/*.c) \
				$(wildcard $(SRC)/*.cpp) \
				)

SRC := oxygine/src

LOCAL_SRC_FILES += \
				$(SRC)/core/android/jniUtils.cpp \
				$(SRC)/core/android/jniHelper.cpp \
				$(SRC)/core/android/HttpRequestJavaTask.cpp

LOCAL_SRC_FILES += \
				$(SRC)/core/gl/NativeTextureGLES.cpp \
				$(SRC)/core/gl/oxgl.cpp \
				$(SRC)/core/gl/VertexDeclarationGL.cpp \
				$(SRC)/core/gl/ShaderProgramGL.cpp \
				$(SRC)/core/gl/VideoDriverGL.cpp \
				$(SRC)/core/gl/VideoDriverGLES20.cpp \


THIRD_PARTY := $(LOCAL_PATH)/oxygine/third_party/android/
OXYGINE_SRC := $(LOCAL_PATH)/oxygine/src

LOCAL_C_INCLUDES := $(OXYGINE_SRC)/ \
					$(LOCAL_PATH)/../SDL/include/ \
					$(THIRD_PARTY)/libjpeg/ \
					$(THIRD_PARTY)/libpng/

LOCAL_CFLAGS := -DUSE_FILE32API -DOX_HAVE_LIBJPEG=1 -DOX_HAVE_LIBPNG=1

LOCAL_EXPORT_CFLAGS := $(LOCAL_CFLAGS)
LOCAL_EXPORT_LDLIBS += -lGLESv2 -llog -lz
LOCAL_EXPORT_C_INCLUDES += $(LOCAL_C_INCLUDES)
LOCAL_WHOLE_STATIC_LIBRARIES := ox_libpng_static ox_libjpeg_static
					
include $(BUILD_STATIC_LIBRARY)

$(call import-module, SDL)