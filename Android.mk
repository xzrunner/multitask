INNER_SAVED_LOCAL_PATH := $(LOCAL_PATH)

LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := multitask

LOCAL_C_INCLUDES  := \

LOCAL_C_INCLUDES  := \
	${MULTITASK_SRC_PATH}/include \
	${CU_SRC_PATH} \

LOCAL_SRC_FILES := \
	$(subst $(LOCAL_PATH)/,,$(shell find $(LOCAL_PATH) -name "*.cpp" -print)) \

LOCAL_CPPFLAGS  := -std=c++11

include $(BUILD_STATIC_LIBRARY)

LOCAL_PATH := $(INNER_SAVED_LOCAL_PATH)