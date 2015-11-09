LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := libidentifyapp
LOCAL_SRC_FILES := libidentifyapp.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libcasdkjni
LOCAL_SRC_FILES := libcasdkjni.so
include $(PREBUILT_SHARED_LIBRARY)

