/*
 * cgeImageHandlerWrapper.cpp
 *
 *  Created on: 2015-12-25
 *      Author: Wang Yang
 *        Mail: admin@wysaid.org
 */

#include "cgeImageHandlerWrapper.h"
#include "cgeTextureUtils.h"
#include "cgeUtilFunctions.h"
#include "cgeMultipleEffects.h"

#include <math.h>

using namespace CGE;

extern "C" {

    JNIEXPORT jlong JNICALL
    Java_org_wysaid_nativePort_CGEImageHandler_nativeCreateHandler(JNIEnv *env, jobject) {
        CGEImageHandlerAndroid *handler = new CGEImageHandlerAndroid();
        return (jlong) handler;
    }

    JNIEXPORT jboolean JNICALL
    Java_org_wysaid_nativePort_CGEImageHandler_nativeInitWithBitmap(JNIEnv *env, jobject, jlong addr,
                                                                    jobject bmp) {
        CGEImageHandlerAndroid *handler = (CGEImageHandlerAndroid *) addr;
        return handler->initWithBitmap(env, bmp, true); //use revision.
    }

    JNIEXPORT jobject JNICALL
    Java_org_wysaid_nativePort_CGEImageHandler_nativeGetResultBitmap(JNIEnv *env, jobject, jlong addr) {
        CGEImageHandlerAndroid *handler = (CGEImageHandlerAndroid *) addr;
        return handler->getResultBitmap(env);
    }

    JNIEXPORT jobject JNICALL
    Java_org_wysaid_nativePort_CGEImageHandler_nativeGetResultBitmapWithResultSize(JNIEnv *env, jobject,
                                                                                   jlong addr,
                                                                                   jstring config,
                                                                                   jfloat outputScale,
                                                                                   jint resultWidth,
                                                                                   jint resultHeight) {
        CGEImageHandlerAndroid *handler = (CGEImageHandlerAndroid *) addr;

        handler->setOutputScale(outputScale);

        handler->clearImageFilters(true);

        handler->revertToKeptResult();

        if (config == nullptr) {
            return nullptr;
        }

        const char *configStr = env->GetStringUTFChars(config, 0);

        if (configStr == nullptr || *configStr == '\0') {
            CGE_LOG_INFO("Using empty filter config.");
        } else {
            CGETexLoadArg texLoadArg;

            texLoadArg.env = env;
            texLoadArg.cls = env->FindClass("org/wysaid/nativePort/CGENativeLibrary");

            CGEMutipleEffectFilter *filter = new CGEMutipleEffectFilter();
            filter->setTextureLoadFunction(cgeGlobalTextureLoadFunc, &texLoadArg);

            if (!filter->initWithEffectString(handler, configStr)) {
                delete filter;
            } else {
                handler->addImageFilter(filter);
                handler->processingFilters();
            }

            filter = nullptr;
        }

        env->ReleaseStringUTFChars(config, configStr);

        return handler->getResultBitmap(env, round(resultWidth * outputScale), round(resultHeight * outputScale));
    }

    JNIEXPORT jobject JNICALL
    Java_org_wysaid_nativePort_CGEImageHandler_createTexture(JNIEnv * env,
                                                            jlong addr,
                                                            jobject bitmap,
                                                            jint width,
                                                            jint height,
                                                            jstring name) {
        void* data;
        int cwidth = width;
        int cheight = height;
        const char* cname = env->GetStringUTFChars(name, 0);

        AndroidBitmap_lockPixels(env, bitmap, &data);

        //This fills out my Texture class
        //GLuint texture = ResourceManager::instance()->createTexture(cname, cwidth, cheight, data);

        AndroidBitmap_unlockPixels(env, bitmap);

        return nullptr;
    }

    JNIEXPORT jobject JNICALL
    Java_org_wysaid_nativePort_CGEImageHandler_nativeGetBlurBitmap(JNIEnv *env, jobject, jlong addr) {
        CGEImageHandlerAndroid *handler = (CGEImageHandlerAndroid *) addr;

        const char *configStr = "@adjust lerp ";

        CGETexLoadArg texLoadArg;

        texLoadArg.env = env;
        texLoadArg.cls = env->FindClass("org/wysaid/nativePort/CGENativeLibrary");

        CGEMutipleEffectFilter *filter = new CGEMutipleEffectFilter();
        filter->setTextureLoadFunction(cgeGlobalTextureLoadFunc, &texLoadArg);

        if (!filter->initWithEffectString(nullptr, configStr)) {
            delete filter;
        } else {
            handler->addImageFilter(filter);
            handler->processingFilters();
        }

        filter = nullptr;

        jobject bitmapToReturn = handler->getResultBitmap(env);

        handler->popImageFilter();

        return bitmapToReturn;
    }

    JNIEXPORT jobject JNICALL
    Java_org_wysaid_nativePort_CGEImageHandler_nativeGetThumbnailWithConfig(JNIEnv *env, jobject,
                                                                            jlong addr,
                                                                            jstring config) {
        CGEImageHandlerAndroid *handler = (CGEImageHandlerAndroid *) addr;

        handler->clearImageFilters(true);

        handler->revertToKeptResult();

        if (config == nullptr) {
            return nullptr;
        }

        const char *configStr = env->GetStringUTFChars(config, 0);

        if (configStr == nullptr || *configStr == '\0') {
            CGE_LOG_INFO("Using empty filter config.");
        } else {
            CGETexLoadArg texLoadArg;

            texLoadArg.env = env;
            texLoadArg.cls = env->FindClass("org/wysaid/nativePort/CGENativeLibrary");

            CGEMutipleEffectFilter *filter = new CGEMutipleEffectFilter();
            filter->setTextureLoadFunction(cgeGlobalTextureLoadFunc, &texLoadArg);

            if (!filter->initWithEffectString(nullptr, configStr)) {
                delete filter;
            } else {
                handler->addImageFilter(filter);
                handler->processingFilters();
            }

            filter = nullptr;
        }

        env->ReleaseStringUTFChars(config, configStr);

        return handler->getResultBitmap(env);
    }

    JNIEXPORT void JNICALL
    Java_org_wysaid_nativePort_CGEImageHandler_nativeSetDrawerRotation(JNIEnv *env, jobject, jlong addr,
                                                                       jfloat rad) {
        CGEImageHandlerAndroid *handler = (CGEImageHandlerAndroid *) addr;
        auto *drawer = handler->getResultDrawer();
        drawer->setRotation(rad);
    }

    JNIEXPORT void JNICALL
    Java_org_wysaid_nativePort_CGEImageHandler_nativeSetDrawerFlipScale(JNIEnv *env, jobject,
                                                                        jlong addr, jfloat x,
                                                                        jfloat y) {
        CGEImageHandlerAndroid *handler = (CGEImageHandlerAndroid *) addr;
        auto *drawer = handler->getResultDrawer();
        drawer->setFlipScale(x, y);
    }

    JNIEXPORT jboolean JNICALL
    Java_org_wysaid_nativePort_CGEImageHandler_nativeSetFilterWithConfig(JNIEnv *env, jobject,
                                                                         jlong addr, jstring config,
                                                                         jboolean shouldCleanOlder,
                                                                         jboolean shouldProcess) {
        CGEImageHandlerAndroid *handler = (CGEImageHandlerAndroid *) addr;

        if (shouldCleanOlder) handler->clearImageFilters(true);
        if (shouldProcess) handler->revertToKeptResult();
        if (config == nullptr) return false;

        const char *configStr = env->GetStringUTFChars(config, 0);

        if (configStr == nullptr || *configStr == '\0') {
            CGE_LOG_INFO("Using empty filter config.");
        } else {
            CGETexLoadArg texLoadArg;

            texLoadArg.env = env;
            texLoadArg.cls = env->FindClass("org/wysaid/nativePort/CGENativeLibrary");

            CGEMutipleEffectFilter *filter = new CGEMutipleEffectFilter();
            filter->setTextureLoadFunction(cgeGlobalTextureLoadFunc, &texLoadArg);

            if (!filter->initWithEffectString(nullptr, configStr)) {
                delete filter;
            } else {
                handler->addImageFilter(filter);

                if (shouldProcess && handler->getTargetTextureID() != 0) {
                    handler->processingFilters();
                }
            }
        }

        env->ReleaseStringUTFChars(config, configStr);

        return true;
    }

    JNIEXPORT void JNICALL
    Java_org_wysaid_nativePort_CGEImageHandler_nativeSetFilterIntensity(JNIEnv *env, jobject,
                                                                        jlong addr, jint progress,
                                                                        jboolean shouldProcess) {
        CGEImageHandlerAndroid *handler = (CGEImageHandlerAndroid *) addr;

        auto &&filters = handler->peekFilters();

        for (auto *filter : filters) {
            filter->setIntensity(progress);
        }

        if (shouldProcess && !filters.empty() && handler->getTargetTextureID() != 0) {
            handler->revertToKeptResult();
            handler->processingFilters();
        }
    }

    JNIEXPORT jboolean JNICALL
    Java_org_wysaid_nativePort_CGEImageHandler_nativeSetFilterValueAtIndex(JNIEnv *env, jobject,
                                                                           jlong addr, jstring value,
                                                                           jint index,
                                                                           jboolean shouldProcess) {
        CGEImageHandlerAndroid *handler = (CGEImageHandlerAndroid *) addr;

        auto &&filters = handler->peekFilters();

        if (index < 0 || filters.empty()) {
            return false;
        }

        CGEImageFilterInterfaceAbstract *filter = nullptr;

        if (filters.size() == 1) {
            auto *mutipleFilter = filters[0];
            auto &&innerFilters = mutipleFilter->getFilters(false);

            if (index >= innerFilters.size()) {
                return false;
            }

            innerFilters[index];
        } else if (index < filters.size()) {
            filter = filters[index];
        } else {
            return false;
        }

        assert(filter != nullptr); //impossible

        CGEMutipleEffectFilter *multiFilter = dynamic_cast<CGEMutipleEffectFilter *>(filter);

        const char *nativeValue = env->GetStringUTFChars(value, 0);

            // Warmth
        if (CGEWarmthFilter *warmthFilter = dynamic_cast<CGEWarmthFilter *>(multiFilter->getFilter())) {
            warmthFilter->setWarmth(atof(nativeValue));
        }
            // Vignette
        else if (CGEVignetteFilter *vignetteFilter = dynamic_cast<CGEVignetteFilter *>(multiFilter->getFilter())) {
            float strength, centerX, centerY, widthMultiplier, heightMultiplier;
            int amountOfArguments = sscanf(nativeValue,
                                           "%f%*c%f%*c%f%*c%f%*c%f",
                                           &strength, &centerX, &centerY, &widthMultiplier,
                                           &heightMultiplier
            );

            if (amountOfArguments == 1) {
                vignetteFilter->setVignetteStrength(strength);
            } else if (amountOfArguments == 4) {
                vignetteFilter->setCenter(centerX, centerY);
                vignetteFilter->setSizeMultiplier(widthMultiplier, heightMultiplier);
            } else if (amountOfArguments == 5) {
                vignetteFilter->setVignetteStrength(strength);
                vignetteFilter->setCenter(centerX, centerY);
                vignetteFilter->setSizeMultiplier(widthMultiplier, heightMultiplier);
            } else {
                CGE_LOG_ERROR("adjusting vignette failed: %s", nativeValue);
                return false;
            }
        }
            // Saturation
        else if (CGESaturationFilter *saturationFilter = dynamic_cast<CGESaturationFilter *>(multiFilter->getFilter())) {
            saturationFilter->setSaturation(atof(nativeValue));
        }
            // Contrast
        else if (CGEContrastFilter *contrastFilter = dynamic_cast<CGEContrastFilter *>(multiFilter->getFilter())) {
            contrastFilter->setContrast(atof(nativeValue));
        }
            // Exposure
        else if (CGEExposureFilter *exposureFilter = dynamic_cast<CGEExposureFilter *>(multiFilter->getFilter())) {
            exposureFilter->setExposure(atof(nativeValue));
        }
            // Shadows
        else if (CGEShadowsFilter *shadowFilter = dynamic_cast<CGEShadowsFilter *>(multiFilter->getFilter())) {
            shadowFilter->setShadows(atof(nativeValue));
        }
            // Highlights
        else if (CGEHighlightsFilter *highlightsFilter = dynamic_cast<CGEHighlightsFilter *>(multiFilter->getFilter())) {
            highlightsFilter->setHighlights(atof(nativeValue));
        }
            // Sharpen
        else if (CGESharpenFilter *sharpenFilter = dynamic_cast<CGESharpenFilter *>(multiFilter->getFilter())) {
            sharpenFilter->setRenderSize(handler->getSourceWidth(), handler->getSourceHeight());
            sharpenFilter->setSharpness(atof(nativeValue));
        }
            // Focus
        else if (CGEFocusFilter *focusFilter = dynamic_cast<CGEFocusFilter *>(multiFilter->getFilter())) {
            int numberOfArguments = countArguments(nativeValue);

            if(numberOfArguments == 1) {
                // show edit circle
                if (strcmp(nativeValue, "true") == 0) {
                    focusFilter->hideEditCircle(true);
                    focusFilter->setEditBorderScale(handler->getCropScale());
                }
                // hide edit circle
                else if (strcmp(nativeValue, "false") == 0) {
                    focusFilter->hideEditCircle(false);
                    focusFilter->setEditBorderScale(handler->getCropScale());
                }
                // focus type
                else if (strlen(nativeValue) == 1) {
                    focusFilter->setFocusType(atoi(nativeValue));
                    handler->setFocusType(focusFilter->type);
                }
                // move focus radius
                else if(strlen(nativeValue) > 1) {
                    focusFilter->moveFocusRadius(atof(nativeValue));
                    handler->setFocusRadius(focusFilter->radius);
                }
            }
            // move focus position
            else if(numberOfArguments == 2) {
                float focusPositionX, focusPositionY;
                sscanf(nativeValue, "%f%*c%f", &focusPositionX, &focusPositionY);

                focusFilter->moveFocusPosition(focusPositionX * handler->getCropScale(), focusPositionY * handler->getCropScale());
                handler->setFocusPosition(focusFilter->xPosition, focusFilter->yPosition);
            }
            // texture 
            else if(numberOfArguments == 3) {
                int textureId, textureWidth, textureHeight;
                sscanf(nativeValue, "%d%*c%d%*c%d", &textureId, &textureWidth, &textureHeight);
                if (textureId != 0) {
                    focusFilter->setBlurTexture(textureId);
                    focusFilter->setAspectRatio((float)textureHeight / (float)textureWidth);

                    handler->setFocusType(focusFilter->type);
                    handler->setFocusPosition(focusFilter->xPosition, focusFilter->yPosition);
                    handler->setFocusRadius(focusFilter->radius);
                }
            }
            // all except texture, used to save current filter
            else if(numberOfArguments == 4) {
                int focusType;
                float focusPositionX, focusPositionY, radius;
                sscanf(nativeValue, "%d%*c%f%*c%f%*c%f", &focusType, &focusPositionX, &focusPositionY, &radius);

                focusFilter->hideEditCircle(true);
                focusFilter->setFocusType(focusType);
                focusFilter->setFocusPosition(focusPositionX, focusPositionY);
                focusFilter->setFocusRadius(radius);

                handler->setFocusType(focusFilter->type);
                handler->setFocusPosition(focusFilter->xPosition, focusFilter->yPosition);
                handler->setFocusRadius(focusFilter->radius);
            }
        }
            // Transformation
        else if (CGETransformationFilter *transformationFilter = dynamic_cast<CGETransformationFilter *>(multiFilter->getFilter())) {
            float m[16];
            float xPos, yPos, cropWidth, cropHeight, centerX, centerY;

            int amountOfArguments = sscanf(nativeValue,
                                           "%f%*c%f%*c%f%*c%f%*c%f%*c%f%*c%f%*c%f%*c%f%*c%f%*c%f%*c%f%*c%f%*c%f%*c%f%*c%f%*c%f%*c%f%*c%f%*c%f%*c%f%*c%f",
                                           m + 0, m + 1, m + 2, m + 3, m + 4, m + 5, m + 6, m + 7,
                                           m + 8, m + 9, m + 10, m + 11, m + 12, m + 13, m + 14, m + 15,
                                           &xPos, &yPos, &cropWidth, &cropHeight, &centerX, &centerY);

            if (amountOfArguments == 22) {
                transformationFilter->setTransformationMatrix(m);
                transformationFilter->setCrop(xPos, yPos, cropWidth, cropHeight);
                handler->setCenter(centerX, centerY);
                handler->setCropScale(getCropScale(cropWidth, cropHeight, handler->getSourceWidth(), handler->getSourceHeight()));
            } else if (amountOfArguments == 20) {
                transformationFilter->setTransformationMatrix(m);
                transformationFilter->setCrop(xPos, yPos, cropWidth, cropHeight);
                handler->setCropScale(getCropScale(cropWidth, cropHeight, handler->getSourceWidth(), handler->getSourceHeight()));
            } else if (amountOfArguments == 16) {
                transformationFilter->setTransformationMatrix(m);
            } else {
                amountOfArguments = sscanf(nativeValue, "%f%*c%f%*c%f%*c%f%*c%f%*c%f", &xPos, &yPos, &cropWidth, &cropHeight, &centerX, &centerY);

                if (amountOfArguments == 6) {
                    transformationFilter->setCrop(xPos, yPos, cropWidth, cropHeight);
                    handler->setCenter(centerX, centerY);
                    handler->setCropScale(getCropScale(cropWidth, cropHeight, handler->getSourceWidth(), handler->getSourceHeight()));
                } else if (amountOfArguments == 4) {
                    transformationFilter->setCrop(xPos, yPos, cropWidth, cropHeight);
                    handler->setCropScale(getCropScale(cropWidth, cropHeight, handler->getSourceWidth(), handler->getSourceHeight()));
                } else {
                    CGE_LOG_ERROR("adjusting transformation failed: %s", nativeValue);
                    return false;
                }
            }
        }
            // LUT
        else if (CGELookupFilter *lookupFilter = dynamic_cast<CGELookupFilter *>(multiFilter->getFilter())) {
            CGETexLoadArg texLoadArg;

            texLoadArg.env = env;
            texLoadArg.cls = env->FindClass("org/wysaid/nativePort/CGENativeLibrary");

            multiFilter->setTextureLoadFunction(cgeGlobalTextureLoadFunc, &texLoadArg);

            if (strcmp(nativeValue, "original.png") == 0) {
                lookupFilter->doNotUse(true);
            } else {
                lookupFilter->doNotUse(false);
                lookupFilter->setLookupTexture(multiFilter->loadResources(nativeValue));
            }
        }
            // the other filters
        else if(filter != nullptr) {
            filter->setIntensity(atof(nativeValue));
        }

        if (shouldProcess && handler->getTargetTextureID() != 0) {
            handler->revertToKeptResult();
            handler->processingFilters();
        }

        return true;
    }

    float getCropScale(int cropWidth, int cropHeight, int sourceWidth, int sourceHeight) {
        float scaleX = cropWidth / (float)sourceWidth;
        float scaleY = cropHeight / (float)sourceHeight;
        return fmax(scaleX, scaleY);
    }

    JNIEXPORT void JNICALL
    Java_org_wysaid_nativePort_CGEImageHandler_nativeDrawResult(JNIEnv *env, jobject, jlong addr) {
        CGEImageHandlerAndroid *handler = (CGEImageHandlerAndroid *) addr;
        handler->drawResult();
    }

    JNIEXPORT void JNICALL
    Java_org_wysaid_nativePort_CGEImageHandler_nativeBindTargetFBO(JNIEnv *env, jobject, jlong addr) {
        CGEImageHandlerAndroid *handler = (CGEImageHandlerAndroid *) addr;
        handler->useImageFBO();
    }

    JNIEXPORT void JNICALL
    Java_org_wysaid_nativePort_CGEImageHandler_nativeSetAsTarget(JNIEnv *env, jobject, jlong addr) {
        CGEImageHandlerAndroid *handler = (CGEImageHandlerAndroid *) addr;
        handler->setAsTarget();
    }

    JNIEXPORT void JNICALL
    Java_org_wysaid_nativePort_CGEImageHandler_nativeSwapBufferFBO(JNIEnv *env, jobject, jlong addr) {
        CGEImageHandlerAndroid *handler = (CGEImageHandlerAndroid *) addr;
        handler->swapBufferFBO();
    }

    JNIEXPORT void JNICALL
    Java_org_wysaid_nativePort_CGEImageHandler_nativeRevertImage(JNIEnv *env, jobject, jlong addr) {
        CGEImageHandlerAndroid *handler = (CGEImageHandlerAndroid *) addr;
        handler->revertToKeptResult();
    }

    JNIEXPORT void JNICALL
    Java_org_wysaid_nativePort_CGEImageHandler_nativeProcessingFilters(JNIEnv *env, jobject,
                                                                       jlong addr) {
        CGEImageHandlerAndroid *handler = (CGEImageHandlerAndroid *) addr;
        handler->processingFilters();
    }

    JNIEXPORT void JNICALL
    Java_org_wysaid_nativePort_CGEImageHandler_nativeRelease(JNIEnv *env, jobject, jlong addr) {
        CGEImageHandlerAndroid *handler = (CGEImageHandlerAndroid *) addr;
        handler->release(env);
        delete handler;
    }

    JNIEXPORT void JNICALL
    Java_org_wysaid_nativePort_CGEImageHandler_nativeProcessWithFilter(JNIEnv *env, jobject, jlong addr,
                                                                       jlong filterAddr) {
        CGEImageHandlerAndroid *handler = (CGEImageHandlerAndroid *) addr;
        handler->processingWithFilter((CGEImageFilterInterfaceAbstract *) filterAddr);
    }

    JNIEXPORT void JNICALL
    Java_org_wysaid_nativePort_CGEImageHandler_nativeSetRenderSize(JNIEnv *env, jobject, jlong addr,
                                                                   jint renderWidth,
                                                                   jint renderHeight) {
        CGEImageHandlerAndroid *handler = (CGEImageHandlerAndroid *) addr;
        handler->setRenderSize(renderWidth, renderHeight);
    }

    JNIEXPORT void JNICALL
    Java_org_wysaid_nativePort_CGEImageHandler_nativeSetSourceSize(JNIEnv *env, jobject, jlong addr,
                                                                   jint sourceWidth,
                                                                   jint sourceHeight) {
        CGEImageHandlerAndroid *handler = (CGEImageHandlerAndroid *) addr;
        handler->setSourceSize(sourceWidth, sourceHeight);
    }

    JNIEXPORT void JNICALL
    Java_org_wysaid_nativePort_CGEImageHandler_nativeSetOutputScale(JNIEnv *env, jobject, jlong addr,
                                                                   jfloat outputScale) {
        CGEImageHandlerAndroid *handler = (CGEImageHandlerAndroid *) addr;
        handler->setOutputScale(outputScale);
    }

    JNIEXPORT jint JNICALL
    Java_org_wysaid_nativePort_CGEImageHandler_nativeGetFocusType(JNIEnv *env, jobject, jlong addr) {
        CGEImageHandlerAndroid *handler = (CGEImageHandlerAndroid *) addr;
        return handler->getFocusType();
    }

    JNIEXPORT jfloat JNICALL
    Java_org_wysaid_nativePort_CGEImageHandler_nativeGetFocusXPosition(JNIEnv *env, jobject, jlong addr) {
        CGEImageHandlerAndroid *handler = (CGEImageHandlerAndroid *) addr;
        return handler->getFocusXPosition();
    }

    JNIEXPORT jfloat JNICALL
    Java_org_wysaid_nativePort_CGEImageHandler_nativeGetFocusYPosition(JNIEnv *env, jobject, jlong addr) {
        CGEImageHandlerAndroid *handler = (CGEImageHandlerAndroid *) addr;
        return handler->getFocusYPosition();
    }

    JNIEXPORT jfloat JNICALL
    Java_org_wysaid_nativePort_CGEImageHandler_nativeGetFocusRadius(JNIEnv *env, jobject, jlong addr) {
        CGEImageHandlerAndroid *handler = (CGEImageHandlerAndroid *) addr;
        return handler->getFocusRadius();
    }

    JNIEXPORT jfloat JNICALL
    Java_org_wysaid_nativePort_CGEImageHandler_nativeGetCropScale(JNIEnv *env, jobject, jlong addr) {
        CGEImageHandlerAndroid *handler = (CGEImageHandlerAndroid *) addr;
        return handler->getCropScale();
    }
}
