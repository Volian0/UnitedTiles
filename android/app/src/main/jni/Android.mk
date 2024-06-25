
LOCAL_PATH := $(call my-dir)/../../../../../..


#SDL2


include $(CLEAR_VARS)
LOCAL_MODULE := SDL2
LOCAL_C_INCLUDES += $(LOCAL_PATH)/include/SDL

LOCAL_SRC_FILES := $(LOCAL_PATH)/lib/android/static/$(TARGET_ARCH_ABI)/libSDL2.a
include $(PREBUILT_STATIC_LIBRARY)


#SDL2_mixer

include $(CLEAR_VARS)
LOCAL_MODULE := SDL2_mixer
LOCAL_PATH2 := $(LOCAL_PATH)/include/SDL_mixer
LOCAL_C_INCLUDES :=                                     \
    $(LOCAL_PATH2)/include                               \
    $(LOCAL_PATH2)/src/                                  \
    $(LOCAL_PATH2)/src/codecs 
LOCAL_C_INCLUDES += $(LOCAL_PATH)/include/SDL

	LOCAL_SRC_FILES := $(wildcard $(LOCAL_PATH2)/src/*.c)
	LOCAL_SRC_FILES += $(wildcard $(LOCAL_PATH2)/src/codecs/*.c)

LOCAL_STATIC_LIBRARIES := SDL2
    LOCAL_CFLAGS += -DMUSIC_WAV
include $(BUILD_STATIC_LIBRARY)

#SDL2_image


include $(CLEAR_VARS)
LOCAL_MODULE := SDL2_image
LOCAL_SRC_FILES := $(LOCAL_PATH)/lib/android/static/$(TARGET_ARCH_ABI)/libSDL2_image.a
include $(PREBUILT_STATIC_LIBRARY)


#SDL2_ttf


include $(CLEAR_VARS)
LOCAL_MODULE := SDL2_ttf
LOCAL_SRC_FILES := $(LOCAL_PATH)/lib/android/static/$(TARGET_ARCH_ABI)/libSDL2_ttf.a
include $(PREBUILT_STATIC_LIBRARY)


#curl


include $(CLEAR_VARS)
LOCAL_MODULE := curl
LOCAL_SRC_FILES := $(LOCAL_PATH)/lib/android/static/$(TARGET_ARCH_ABI)/libcurl.a
include $(PREBUILT_STATIC_LIBRARY)


#freetype


include $(CLEAR_VARS)
LOCAL_MODULE := freetype
LOCAL_SRC_FILES := $(LOCAL_PATH)/lib/android/static/$(TARGET_ARCH_ABI)/libfreetype.a
include $(PREBUILT_STATIC_LIBRARY)


#UnitedTiles

$(call import-module,android/cpufeatures)
include $(CLEAR_VARS)
LOCAL_MODULE := UnitedTiles
LOCAL_C_INCLUDES := $(LOCAL_PATH)/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/include/SDL_mixer
LOCAL_C_INCLUDES += $(LOCAL_PATH)/include/SDL
LOCAL_C_INCLUDES += $(LOCAL_PATH)/include/SDL_mixer/include
LOCAL_SRC_FILES := $(wildcard $(LOCAL_PATH)/src/*.cpp)
LOCAL_SRC_FILES += $(wildcard $(LOCAL_PATH)/src/tiles/*.cpp)
LOCAL_SRC_FILES += $(wildcard $(LOCAL_PATH)/src/ui/*.cpp)


#LOCAL_CPPFLAGS := -std=c++20
LOCAL_STATIC_LIBRARIES += SDL2
LOCAL_STATIC_LIBRARIES += SDL2_mixer
LOCAL_STATIC_LIBRARIES += SDL2_image
LOCAL_STATIC_LIBRARIES += SDL2_ttf
LOCAL_STATIC_LIBRARIES += freetype
LOCAL_STATIC_LIBRARIES += curl
LOCAL_STATIC_LIBRARIES += cpufeatures
LOCAL_LDLIBS := -ldl -lGLESv1_CM -lGLESv2 -lOpenSLES -llog -landroid

include $(BUILD_SHARED_LIBRARY)

