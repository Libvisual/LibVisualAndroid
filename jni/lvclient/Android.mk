LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_C_INCLUDES += $(LV_C_INCLUDES)
LOCAL_CFLAGS     += $(LV_CFLAGS)
LOCAL_CXXFLAGS   += $(LV_CXXFLAGS)
LOCAL_CPPFLAGS   += $(LV_CPPFLAGS)


LOCAL_MODULE           += lvclient
LOCAL_SRC_FILES        += lvclient.c stats.c
LOCAL_SHARED_LIBRARIES += visual
LOCAL_LDLIBS           += -lm -llog -ljnigraphics

include $(BUILD_SHARED_LIBRARY)
