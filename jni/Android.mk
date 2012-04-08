
WARNING_CFLAGS := -Wall -Wstrict-aliasing -Wcast-align -Waddress -Wmissing-braces -Wimplicit -Wunused -Wno-unused-variable

LV_C_INCLUDES  := jni/libvisual/libvisual
LV_CFLAGS      := $(WARNING_CFLAGS)
LV_CXXFLAGS    :=
LV_CPPFLAGS    := -fexceptions



ifeq ($(TARGET_ARCH_ABI),armeabi)
#    LV_CFLAGS += -march=armv6 -mfp=vfp -D__ARM_ARCH_6__ $(WARNING_FLAGS)
endif
        
ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)
    LV_CFLAGS += -DHAVE_NEON=1 -mfloat-abi=softfp -mfpu=neon
endif
            
ifeq ($(TARGET_ARCH_ABI),x86)
    LV_CFLAGS += -DVISUAL_ARCH_X86=1 $(WARNING_FLAGS)
endif
                
                
include $(call all-subdir-makefiles)
