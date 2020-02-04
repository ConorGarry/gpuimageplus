/*
 * cgeImageHandlerWrapper.h
 *
 *  Created on: 2015-12-25
 *      Author: Wang Yang
 *        Mail: admin@wysaid.org
 */

#ifndef _CGE_IMAGEHANDLER_WRAPPER_H_
#define _CGE_IMAGEHANDLER_WRAPPER_H_

#include "jni.h"

#include <stdlib.h>

#include "cgeWarmthAdjust.h"
#include "cgeVignetteAdjust.h"
#include "cgeSaturationAdjust.h"
#include "cgeContrastAdjust.h"
#include "cgeExposureAdjust.h"
#include "cgeShadowsAdjust.h"
#include "cgeHighlightsAdjust.h"
#include "cgeSharpenAdjust.h"
#include "cgeLookupFilter.h"
#include "cgeRotationAdjust.h"
#include "cgeTransformationAdjust.h"
#include "cgeFocusAdjust.h"

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jlong JNICALL Java_org_wysaid_nativePort_CGEImageHandler_nativeCreateHandler
        (JNIEnv *, jobject);

JNIEXPORT jboolean JNICALL Java_org_wysaid_nativePort_CGEImageHandler_nativeInitWithBitmap
        (JNIEnv *, jobject, jlong, jobject);

JNIEXPORT jobject JNICALL Java_org_wysaid_nativePort_CGEImageHandler_nativeGetResultBitmap
        (JNIEnv *, jobject, jlong);

JNIEXPORT jobject JNICALL Java_org_wysaid_nativePort_CGEImageHandler_nativeGetResultBitmapWithResultSize
        (JNIEnv *, jobject, jlong, jstring, jfloat, jint, jint);

JNIEXPORT jobject JNICALL Java_org_wysaid_nativePort_CGEImageHandler_nativeGetBlurBitmap
        (JNIEnv *, jobject, jlong);

JNIEXPORT jobject JNICALL Java_org_wysaid_nativePort_CGEImageHandler_createTexture
        (JNIEnv *, jlong, jobject, jint, jint, jstring);

JNIEXPORT jobject JNICALL Java_org_wysaid_nativePort_CGEImageHandler_nativeGetThumbnailWithConfig
        (JNIEnv *, jobject, jlong, jstring);

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEImageHandler_nativeSetDrawerRotation
        (JNIEnv *, jobject, jlong, jfloat);

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEImageHandler_nativeSetDrawerFlipScale
        (JNIEnv *, jobject, jlong, jfloat, jfloat);

JNIEXPORT jboolean JNICALL Java_org_wysaid_nativePort_CGEImageHandler_nativeSetFilterWithConfig
        (JNIEnv *, jobject, jlong, jstring, jboolean, jboolean);

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEImageHandler_nativeSetFilterIntensity
        (JNIEnv *, jobject, jlong, jint, jboolean);

JNIEXPORT jboolean JNICALL Java_org_wysaid_nativePort_CGEImageHandler_nativeSetFilterValueAtIndex
        (JNIEnv *, jobject, jlong, jstring, jint, jboolean);

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEImageHandler_nativeDrawResult
        (JNIEnv *, jobject, jlong);

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEImageHandler_nativeBindTargetFBO
        (JNIEnv *, jobject, jlong);

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEImageHandler_nativeSetAsTarget
        (JNIEnv *, jobject, jlong);

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEImageHandler_nativeSwapBufferFBO
        (JNIEnv *, jobject, jlong);

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEImageHandler_nativeRevertImage
        (JNIEnv *, jobject, jlong);

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEImageHandler_nativeProcessingFilters
        (JNIEnv *, jobject, jlong);

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEImageHandler_nativeProcessWithFilter
        (JNIEnv *, jobject, jlong, jlong);

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEImageHandler_nativeSetRenderSize
        (JNIEnv *, jobject, jlong, jint, jint);

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEImageHandler_nativeSetSourceSize
        (JNIEnv *, jobject, jlong, jint, jint);

JNIEXPORT jfloat JNICALL Java_org_wysaid_nativePort_CGEImageHandler_nativeGetFocusXPosition
        (JNIEnv *, jobject, jlong);

JNIEXPORT jfloat JNICALL Java_org_wysaid_nativePort_CGEImageHandler_nativeGetFocusXPosition
        (JNIEnv *, jobject, jlong);

JNIEXPORT jfloat JNICALL Java_org_wysaid_nativePort_CGEImageHandler_nativeGetCropScale
        (JNIEnv *, jobject, jlong);

////////////////////////////////////

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGEImageHandler_nativeRelease
        (JNIEnv *, jobject, jlong);

///////////static methods ///////

float getCropScale(int cropWidth, int cropHeight, int sourceWidth, int sourceHeight);

#ifdef __cplusplus
}
#endif

#endif