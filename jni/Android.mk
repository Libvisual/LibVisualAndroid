
WARNING_CFLAGS      := -Wall -Wstrict-aliasing -Wcast-align -Waddress -Wmissing-braces -Wimplicit -Wunused -Wno-unused-variable



LV_C_INCLUDES       += jni/libvisual/libvisual
LV_CFLAGS           += $(WARNING_CFLAGS)
LV_CXXFLAGS         +=
LV_CPPFLAGS         += -std=gnu++0x
LV_LDFLAGS          +=
LV_LDLIBS           +=
LV_SHARED_LIBRARIES +=
LV_STATIC_LIBRARIES += 

# uncomment this for profiling
#TARGET_thumb_release_CFLAGS := $(filter-out -ffunction-sections,$(TARGET_thumb_release_CFLAGS))
#TARGET_thumb_release_CFLAGS := $(filter-out -fomit-frame-pointer,$(TARGET_thumb_release_CFLAGS))
#TARGET_arm_release_CFLAGS := $(filter-out -ffunction-sections,$(TARGET_arm_release_CFLAGS))
#TARGET_arm_release_CFLAGS := $(filter-out -fomit-frame-pointer,$(TARGET_arm_release_CFLAGS))
#TARGET_CFLAGS := $(filter-out -ffunction-sections,$(TARGET_CFLAGS))
#LV_LDLIBS           += -llog
#LV_LDFLAGS          += -pg --coverage
#LV_CFLAGS           += -pg --coverage -DPROFILING
#LV_STATIC_LIBRARIES += andprof

#############################################################################
ifeq ($(TARGET_ARCH_ABI),armeabi)
#    LV_CFLAGS += -march=armv6 -mfp=vfp -D__ARM_ARCH_6__
endif
        
ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)
    LV_CFLAGS += -DHAVE_NEON=1 -mfloat-abi=softfp -mfpu=neon
endif
            
ifeq ($(TARGET_ARCH_ABI),x86)
    LV_CFLAGS += -DVISUAL_ARCH_X86=1 $(WARNING_FLAGS)
endif
                
                
include $(call all-subdir-makefiles)

# profiling
#include jni/android-ndk-profiler.mk
