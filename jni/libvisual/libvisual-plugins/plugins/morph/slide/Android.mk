LOCAL_PATH := $(call my-dir)



#### slide_left
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

LOCAL_MODULE            := morph_slide_left
LOCAL_SRC_FILES         := morph_slide_left.c common.c
LOCAL_LDLIBS            +=
LOCAL_SHARED_LIBRARIES  += visual

include $(BUILD_SHARED_LIBRARY)



#### slide_right
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

LOCAL_MODULE            := morph_slide_right
LOCAL_SRC_FILES         := morph_slide_right.c common.c
LOCAL_LDLIBS            +=
LOCAL_SHARED_LIBRARIES  += visual

include $(BUILD_SHARED_LIBRARY)



#### slide_up
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

LOCAL_MODULE            := morph_slide_up
LOCAL_SRC_FILES         := morph_slide_up.c common.c
LOCAL_LDLIBS            +=
LOCAL_SHARED_LIBRARIES  += visual

include $(BUILD_SHARED_LIBRARY)



#### slide_down
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

LOCAL_MODULE            := morph_slide_down
LOCAL_SRC_FILES         := morph_slide_down.c common.c
LOCAL_LDLIBS            +=
LOCAL_SHARED_LIBRARIES  += visual

include $(BUILD_SHARED_LIBRARY)

