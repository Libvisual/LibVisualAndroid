LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_ARM_MODE          += $(LV_ARM_MODE)
LOCAL_C_INCLUDES        += $(LV_C_INCLUDES)
LOCAL_CFLAGS            += $(LV_CFLAGS)
LOCAL_CXXFLAGS          += $(LV_CXXFLAGS)
LOCAL_CPPFLAGS          += $(LV_CPPFLAGS)
LOCAL_LDFLAGS           += $(LV_LDFLAGS)
LOCAL_LDLIBS            += $(LV_LDLIBS)
LOCAL_SHARED_LIBRARIES  += $(LV_SHARED_LIBRARIES)
LOCAL_STATIC_LIBRARIES  += $(LV_STATIC_LIBRARIES)

LOCAL_MODULE            := visual
LOCAL_SRC_FILES         := \
	$(addprefix /, $(notdir $(wildcard $(LOCAL_PATH)/*.c) $(wildcard $(LOCAL_PATH)/*.cpp))) \
	$(addprefix /private/, $(notdir $(wildcard $(LOCAL_PATH)/private/*.c) $(wildcard $(LOCAL_PATH)/private/*.cpp))) \
    $(addprefix /private/posix/, $(notdir $(wildcard $(LOCAL_PATH)/private/posix/*.c) $(wildcard $(LOCAL_PATH)/private/posix/*.cpp)))
LOCAL_LDLIBS            += -ldl -lm -llog
LOCAL_SHARED_LIBRARIES  += cpufeatures

include $(BUILD_SHARED_LIBRARY)
$(call import-module,cpufeatures)
