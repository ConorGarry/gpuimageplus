/*
* cgeNativeLibrary.h
*
*  Created on: 2015-7-9
*      Author: Wang Yang
*        Mail: admin@wysaid.org
*/

/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class org_wysaid_nativePort_CGENativeLibrary */

#ifndef _Included_org_wysaid_nativePort_CGENativeLibrary
#define _Included_org_wysaid_nativePort_CGENativeLibrary
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     org_wysaid_nativePort_CGENativeLibrary
 * Method:    filterImage_MultipleEffects
 * Signature: (Landroid/graphics/Bitmap;Ljava/lang/String;F)Landroid/graphics/Bitmap;
 */
JNIEXPORT jobject JNICALL
Java_org_wysaid_nativePort_CGENativeLibrary_cgeFilterImage_1MultipleEffects
        (JNIEnv *, jclass, jobject, jstring, jfloat);

/*
 * Class:     org_wysaid_nativePort_CGENativeLibrary
 * Method:    filterImage_MultipleEffectsWriteBack
 * Signature: (Landroid/graphics/Bitmap;Ljava/lang/String;F)V
 */
JNIEXPORT void JNICALL
Java_org_wysaid_nativePort_CGENativeLibrary_cgeFilterImage_1MultipleEffectsWriteBack
        (JNIEnv *, jclass, jobject, jstring, jfloat);

/////// 新增加 2016-1-5 //////////

JNIEXPORT jlong JNICALL Java_org_wysaid_nativePort_CGENativeLibrary_cgeCreateFilterWithConfig
        (JNIEnv *, jclass, jstring);

JNIEXPORT void JNICALL Java_org_wysaid_nativePort_CGENativeLibrary_cgeDeleteFilterWithAddress
        (JNIEnv *, jclass, jlong);

JNIEXPORT jlong JNICALL Java_org_wysaid_nativePort_CGENativeLibrary_cgeCreateBlendFilter
        (JNIEnv *, jclass, jint, jint, jint, jint, jint, jfloat);

#ifdef __cplusplus
}
#endif
#endif
