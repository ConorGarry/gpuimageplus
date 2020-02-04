#
# Created on: 2015-7-9
#     Author: Wang Yang
#       Mail: admin@wysaid.org
#  Edited by: Niek Akerboom
#

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := CGE

#*********************** CGE Library ****************************

CGE_ROOT=$(LOCAL_PATH)

CGE_SOURCE=$(CGE_ROOT)/cge

CGE_INCLUDE=$(CGE_ROOT)/include

#### CGE Library headers ###########
LOCAL_C_INCLUDES := \
					$(CGE_ROOT)/interface \
					$(CGE_INCLUDE) \
					$(CGE_INCLUDE)/filters \


#### CGE Library native source  ###########

LOCAL_SRC_FILES :=  \
			$(CGE_SOURCE)/common/cgeCommonDefine.cpp \
			$(CGE_SOURCE)/common/cgeGLFunctions.cpp \
			$(CGE_SOURCE)/common/cgeImageFilter.cpp \
			$(CGE_SOURCE)/common/cgeImageHandler.cpp \
			$(CGE_SOURCE)/common/cgeShaderFunctions.cpp \
			$(CGE_SOURCE)/common/cgeGlobal.cpp \
			$(CGE_SOURCE)/common/cgeTextureUtils.cpp \
			\
			\
			$(CGE_SOURCE)/filters/cgeFilterBasic.cpp \
			\
			$(CGE_SOURCE)/filters/cgeContrastAdjust.cpp \
			$(CGE_SOURCE)/filters/cgeExposureAdjust.cpp \
			$(CGE_SOURCE)/filters/cgeHighlightsAdjust.cpp \
			$(CGE_SOURCE)/filters/cgeLookupFilter.cpp \
			$(CGE_SOURCE)/filters/cgeSaturationAdjust.cpp \
			$(CGE_SOURCE)/filters/cgeShadowsAdjust.cpp \
			$(CGE_SOURCE)/filters/cgeSharpenAdjust.cpp \
			$(CGE_SOURCE)/filters/cgeVignetteAdjust.cpp \
			$(CGE_SOURCE)/filters/cgeWarmthAdjust.cpp \
			$(CGE_SOURCE)/filters/cgeRotationAdjust.cpp \
			$(CGE_SOURCE)/filters/cgeTransformationAdjust.cpp \
			$(CGE_SOURCE)/filters/cgeFocusAdjust.cpp \
			$(CGE_SOURCE)/filters/cgeLerpblurFilter.cpp \
			\
			$(CGE_SOURCE)/filters/cgeDataParsingEngine.cpp \
			$(CGE_SOURCE)/filters/cgeMultipleEffects.cpp \
			$(CGE_SOURCE)/filters/cgeAdvancedEffects.cpp \
			\
			$(CGE_SOURCE)/extends/cgeThread.cpp \
			\
			$(CGE_ROOT)/interface/cgeNativeLibrary.cpp \
			$(CGE_ROOT)/interface/cgeFFmpegNativeLibrary.cpp \
			$(CGE_ROOT)/interface/cgeSharedGLContext.cpp \
			$(CGE_ROOT)/interface/cgeFrameRenderer.cpp \
			$(CGE_ROOT)/interface/cgeFrameRendererWrapper.cpp \
			$(CGE_ROOT)/interface/cgeFrameRecorder.cpp \
			$(CGE_ROOT)/interface/cgeFrameRecorderWrapper.cpp \
			$(CGE_ROOT)/interface/cgeVideoEncoder.cpp \
			$(CGE_ROOT)/interface/cgeUtilFunctions.cpp \
			$(CGE_ROOT)/interface/cgeVideoDecoder.cpp \
			$(CGE_ROOT)/interface/cgeVideoPlayer.cpp \
			$(CGE_ROOT)/interface/cgeImageHandlerAndroid.cpp \
			$(CGE_ROOT)/interface/cgeImageHandlerWrapper.cpp \


LOCAL_CPPFLAGS := -frtti -std=c++11
LOCAL_LDLIBS :=  -llog -lEGL -lGLESv2 -ljnigraphics -latomic

# 'CGE_USE_VIDEO_MODULE' determines if the project should compile with ffmpeg.

ifdef CGE_USE_VIDEO_MODULE

VIDEO_MODULE_DEFINE = -D_CGE_USE_FFMPEG_

endif

ifndef CGE_RELEASE_MODE

BUILD_MODE = -D_CGE_LOGS_

endif

LOCAL_CFLAGS    := ${VIDEO_MODULE_DEFINE} ${BUILD_MODE} -DANDROID_NDK -DCGE_LOG_TAG=\"libCGE\" -DCGE_TEXTURE_PREMULTIPLIED=1 -D__STDC_CONSTANT_MACROS -D_CGE_DISABLE_GLOBALCONTEXT_ -O3 -ffast-math -D_CGE_ONLY_FILTERS_

ifndef CGE_USE_VIDEO_MODULE

#LOCAL_CFLAGS := $(LOCAL_CFLAGS) -D_CGE_ONLY_FILTERS_

include $(BUILD_SHARED_LIBRARY)

else 

LOCAL_SHARED_LIBRARIES := ffmpeg

include $(BUILD_SHARED_LIBRARY)

################################

# include $(CLEAR_VARS)
# LOCAL_MODULE := x264
# LOCAL_CFLAGS := -march=armv7-a -mfloat-abi=softfp -mfpu=neon -O3 -ffast-math -funroll-loops
# LOCAL_SRC_FILES := ffmpeg/libx264.142.so
# #LOCAL_EXPORT_C_INCLUDES := $(CGE_ROOT)/ffmpeg
# include $(PREBUILT_SHARED_LIBRARY)

###############################

include $(CLEAR_VARS)
LOCAL_MODULE := ffmpeg
LOCAL_CFLAGS := -mfloat-abi=softfp -mfpu=vfp -O3 -ffast-math -funroll-loops -fPIC
ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)
LOCAL_CFLAGS := $(LOCAL_CFLAGS) march=armv7-a -mfpu=neon
endif
LOCAL_SRC_FILES := ffmpeg/$(TARGET_ARCH_ABI)/libffmpeg.so
LOCAL_EXPORT_C_INCLUDES := $(CGE_ROOT)/ffmpeg

# LOCAL_SHARED_LIBRARIES := x264

include $(PREBUILT_SHARED_LIBRARY)

endif

###############################

# Call user_id defined module
include $(CLEAR_VARS)
include $(CGE_ROOT)/source/source.mk