LOCAL_PATH := $(call my-dir)/../../../../../..


#SDL2


include $(CLEAR_VARS)
LOCAL_MODULE := SDL2
LOCAL_SRC_FILES := $(LOCAL_PATH)/lib/android/static/$(TARGET_ARCH_ABI)/libSDL2.a
include $(PREBUILT_STATIC_LIBRARY)


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


#Fluidsynth


include $(CLEAR_VARS)
LOCAL_MODULE := libcharset
LOCAL_SRC_FILES := $(LOCAL_PATH)/lib/android/dynamic/$(TARGET_ARCH_ABI)/libcharset.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libffi
LOCAL_SRC_FILES := $(LOCAL_PATH)/lib/android/dynamic/$(TARGET_ARCH_ABI)/libffi.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libFLAC
LOCAL_SRC_FILES := $(LOCAL_PATH)/lib/android/dynamic/$(TARGET_ARCH_ABI)/libFLAC.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libfluidsynth
LOCAL_SRC_FILES := $(LOCAL_PATH)/lib/android/dynamic/$(TARGET_ARCH_ABI)/libfluidsynth.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libfluidsynth-assetloader
LOCAL_SRC_FILES := $(LOCAL_PATH)/lib/android/dynamic/$(TARGET_ARCH_ABI)/libfluidsynth-assetloader.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libgio-2.0
LOCAL_SRC_FILES := $(LOCAL_PATH)/lib/android/dynamic/$(TARGET_ARCH_ABI)/libgio-2.0.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libglib-2.0
LOCAL_SRC_FILES := $(LOCAL_PATH)/lib/android/dynamic/$(TARGET_ARCH_ABI)/libglib-2.0.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libgmodule-2.0
LOCAL_SRC_FILES := $(LOCAL_PATH)/lib/android/dynamic/$(TARGET_ARCH_ABI)/libgmodule-2.0.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libgobject-2.0
LOCAL_SRC_FILES := $(LOCAL_PATH)/lib/android/dynamic/$(TARGET_ARCH_ABI)/libgobject-2.0.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libgthread-2.0
LOCAL_SRC_FILES := $(LOCAL_PATH)/lib/android/dynamic/$(TARGET_ARCH_ABI)/libgthread-2.0.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libiconv
LOCAL_SRC_FILES := $(LOCAL_PATH)/lib/android/dynamic/$(TARGET_ARCH_ABI)/libiconv.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libintl
LOCAL_SRC_FILES := $(LOCAL_PATH)/lib/android/dynamic/$(TARGET_ARCH_ABI)/libintl.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := liboboe
LOCAL_SRC_FILES := $(LOCAL_PATH)/lib/android/dynamic/$(TARGET_ARCH_ABI)/liboboe.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libogg
LOCAL_SRC_FILES := $(LOCAL_PATH)/lib/android/dynamic/$(TARGET_ARCH_ABI)/libogg.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libsndfile
LOCAL_SRC_FILES := $(LOCAL_PATH)/lib/android/dynamic/$(TARGET_ARCH_ABI)/libsndfile.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libvorbis
LOCAL_SRC_FILES := $(LOCAL_PATH)/lib/android/dynamic/$(TARGET_ARCH_ABI)/libvorbis.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libvorbisenc
LOCAL_SRC_FILES := $(LOCAL_PATH)/lib/android/dynamic/$(TARGET_ARCH_ABI)/libvorbisenc.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libvorbisfile
LOCAL_SRC_FILES := $(LOCAL_PATH)/lib/android/dynamic/$(TARGET_ARCH_ABI)/libvorbisfile.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := preloadable_libiconv
LOCAL_SRC_FILES := $(LOCAL_PATH)/lib/android/dynamic/$(TARGET_ARCH_ABI)/preloadable_libiconv.so
include $(PREBUILT_SHARED_LIBRARY)


#UnitedTiles


include $(CLEAR_VARS)
LOCAL_MODULE := UnitedTiles
LOCAL_C_INCLUDES := $(LOCAL_PATH)/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)
LOCAL_SRC_FILES := $(wildcard $(LOCAL_PATH)/src/*.cpp)
LOCAL_CPPFLAGS := -std=c++17
LOCAL_STATIC_LIBRARIES += SDL2
LOCAL_STATIC_LIBRARIES += SDL2_image
LOCAL_STATIC_LIBRARIES += SDL2_ttf
LOCAL_STATIC_LIBRARIES += freetype
LOCAL_STATIC_LIBRARIES += curl
LOCAL_STATIC_LIBRARIES += cpufeatures
LOCAL_LDLIBS := -ldl -lGLESv1_CM -lGLESv2 -lOpenSLES -llog -landroid

LOCAL_SHARED_LIBRARIES += libcharset
LOCAL_SHARED_LIBRARIES += libffi
LOCAL_SHARED_LIBRARIES += libFLAC
LOCAL_SHARED_LIBRARIES += libfluidsynth
LOCAL_SHARED_LIBRARIES += libfluidsynth-assetloader
LOCAL_SHARED_LIBRARIES += libgio-2.0
LOCAL_SHARED_LIBRARIES += libglib-2.0
LOCAL_SHARED_LIBRARIES += libgmodule-2.0
LOCAL_SHARED_LIBRARIES += libgobject-2.0
LOCAL_SHARED_LIBRARIES += libgthread-2.0
LOCAL_SHARED_LIBRARIES += libiconv
LOCAL_SHARED_LIBRARIES += libintl
LOCAL_SHARED_LIBRARIES += liboboe
LOCAL_SHARED_LIBRARIES += libogg
LOCAL_SHARED_LIBRARIES += libsndfile
LOCAL_SHARED_LIBRARIES += libvorbis
LOCAL_SHARED_LIBRARIES += libvorbisenc
LOCAL_SHARED_LIBRARIES += libvorbisfile
LOCAL_SHARED_LIBRARIES += preloadable_libiconv

include $(BUILD_SHARED_LIBRARY)

$(call import-module,android/cpufeatures)