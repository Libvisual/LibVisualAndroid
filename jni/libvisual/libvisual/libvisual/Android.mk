LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_C_INCLUDES += $(LV_C_INCLUDES)
LOCAL_CFLAGS     += $(LV_CFLAGS)
LOCAL_CXXFLAGS   += $(LV_CXXFLAGS)
LOCAL_CPPFLAGS   += $(LV_CPPFLAGS)

LOCAL_MODULE           := visual
LOCAL_SRC_FILES        := $(addprefix /, $(notdir $(wildcard $(LOCAL_PATH)/*.c) $(wildcard $(LOCAL_PATH)/*.cpp)))
LOCAL_LDLIBS           := -ldl -lm -llog -ljnigraphics
LOCAL_SHARED_LIBRARIES := cpufeatures

include $(BUILD_SHARED_LIBRARY)
$(call import-module,cpufeatures)
