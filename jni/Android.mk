LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := gl_measure
LOCAL_SRC_FILES := main.cpp TestGL.cpp ConfigEGL.cpp gl_measure.cpp
#LOCAL_LDLIBS    := -llog -landroid -lEGL -lGLESv1_CM
LOCAL_LDLIBS   := -llog -landroid -lEGL -lGLESv2
LOCAL_STATIC_LIBRARIES := android_native_app_glue

include $(BUILD_SHARED_LIBRARY)

$(call import-module,android/native_app_glue)
