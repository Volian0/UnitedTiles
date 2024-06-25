
LOCAL_PATH := $(call my-dir)/../../../../../..


#SDL2


include $(CLEAR_VARS)
LOCAL_MODULE := SDL2
LOCAL_PATH2 := $(LOCAL_PATH)/include/SDL

LOCAL_C_INCLUDES := $(LOCAL_PATH2)/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/include/SDL/include

LOCAL_EXPORT_C_INCLUDES := $(LOCAL_C_INCLUDES)

LOCAL_SRC_FILES :=  $(wildcard $(LOCAL_PATH2)/src/*.c) \
	$(wildcard $(LOCAL_PATH2)/src/audio/*.c) \
	$(wildcard $(LOCAL_PATH2)/src/audio/android/*.c) \
	$(wildcard $(LOCAL_PATH2)/src/audio/dummy/*.c) \
	$(wildcard $(LOCAL_PATH2)/src/audio/aaudio/*.c) \
	$(wildcard $(LOCAL_PATH2)/src/audio/openslES/*.c) \
	$(LOCAL_PATH2)/src/atomic/SDL_atomic.c.arm \
	$(LOCAL_PATH2)/src/atomic/SDL_spinlock.c.arm \
	$(wildcard $(LOCAL_PATH2)/src/core/android/*.c) \
	$(wildcard $(LOCAL_PATH2)/src/cpuinfo/*.c) \
	$(wildcard $(LOCAL_PATH2)/src/dynapi/*.c) \
	$(wildcard $(LOCAL_PATH2)/src/events/*.c) \
	$(wildcard $(LOCAL_PATH2)/src/file/*.c) \
	$(wildcard $(LOCAL_PATH2)/src/haptic/*.c) \
	$(wildcard $(LOCAL_PATH2)/src/haptic/android/*.c) \
	$(wildcard $(LOCAL_PATH2)/src/hidapi/*.c) \
	$(wildcard $(LOCAL_PATH2)/src/hidapi/android/*.cpp) \
	$(wildcard $(LOCAL_PATH2)/src/joystick/*.c) \
	$(wildcard $(LOCAL_PATH2)/src/joystick/android/*.c) \
	$(wildcard $(LOCAL_PATH2)/src/joystick/hidapi/*.c) \
	$(wildcard $(LOCAL_PATH2)/src/joystick/virtual/*.c) \
	$(wildcard $(LOCAL_PATH2)/src/loadso/dlopen/*.c) \
	$(wildcard $(LOCAL_PATH2)/src/locale/*.c) \
	$(wildcard $(LOCAL_PATH2)/src/locale/android/*.c) \
	$(wildcard $(LOCAL_PATH2)/src/misc/*.c) \
	$(wildcard $(LOCAL_PATH2)/src/misc/android/*.c) \
	$(wildcard $(LOCAL_PATH2)/src/power/*.c) \
	$(wildcard $(LOCAL_PATH2)/src/power/android/*.c) \
	$(wildcard $(LOCAL_PATH2)/src/filesystem/android/*.c) \
	$(wildcard $(LOCAL_PATH2)/src/sensor/*.c) \
	$(wildcard $(LOCAL_PATH2)/src/sensor/android/*.c) \
	$(wildcard $(LOCAL_PATH2)/src/render/*.c) \
	$(wildcard $(LOCAL_PATH2)/src/render/*/*.c) \
	$(wildcard $(LOCAL_PATH2)/src/stdlib/*.c) \
	$(wildcard $(LOCAL_PATH2)/src/thread/*.c) \
	$(wildcard $(LOCAL_PATH2)/src/thread/pthread/*.c) \
	$(wildcard $(LOCAL_PATH2)/src/timer/*.c) \
	$(wildcard $(LOCAL_PATH2)/src/timer/unix/*.c) \
	$(wildcard $(LOCAL_PATH2)/src/video/*.c) \
	$(wildcard $(LOCAL_PATH2)/src/video/android/*.c) \
	$(wildcard $(LOCAL_PATH2)/src/video/yuv2rgb/*.c) \
	$(wildcard $(LOCAL_PATH2)/src/test/*.c)

LOCAL_CFLAGS += -DGL_GLEXT_PROTOTYPES
LOCAL_CFLAGS += \
	-Wall -Wextra \
	-Wdocumentation \
	-Wmissing-prototypes \
	-Wunreachable-code-break \
	-Wunneeded-internal-declaration \
	-Wmissing-variable-declarations \
	-Wfloat-conversion \
	-Wshorten-64-to-32 \
	-Wunreachable-code-return \
	-Wshift-sign-overflow \
	-Wstrict-prototypes \
	-Wkeyword-macro \

LOCAL_CFLAGS += -Wno-unused-parameter -Wno-sign-compare

LOCAL_CXXFLAGS += -std=gnu++11

LOCAL_LDLIBS := -ldl -lGLESv1_CM -lGLESv2 -lOpenSLES -llog -landroid

LOCAL_LDFLAGS := -Wl,--no-undefined

ifeq ($(NDK_DEBUG),1)
    cmd-strip :=
endif

LOCAL_STATIC_LIBRARIES := cpufeatures

LOCAL_EXPORT_LDLIBS := -ldl -lGLESv1_CM -lGLESv2 -llog -landroid


include $(BUILD_STATIC_LIBRARY)


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
	    LOCAL_CFLAGS += -DMUSIC_OGG -DOGG_USE_STB

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

$(call import-module,android/cpufeatures)
