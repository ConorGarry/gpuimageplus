/*
 * cgeImageHandlerAndroid.h
 *
 *  Created on: 2015-12-20
 *      Author: Wang Yang
 *        Mail: admin@wysaid.org
 *	 Edited by: Niek Akerboom
 */

#ifndef _CGE_IMAGEHANDLER_ANDROID_H_
#define _CGE_IMAGEHANDLER_ANDROID_H_

#include "cgeImageHandler.h"

namespace CGE {
    class JniBitmap {
    public:
        uint32_t *_storedBitmapPixels;
        AndroidBitmapInfo _bitmapInfo;

        JniBitmap() {
            _storedBitmapPixels = NULL;
        }
    };

    class CGEImageHandlerAndroid : public CGE::CGEImageHandler {
        int renderWidth, renderHeight, sourceWidth, sourceHeight;
        int cropXPos, cropYPos, cropWidth, cropHeight;
        float centerX, centerY;

        int focusType;
        float focusXPosition, focusYPosition, focusRadius;

        float cropScale = 1.0;

        int outputWidth, outputHeight;
        float outputScale;

    public:
        CGEImageHandlerAndroid();

        ~CGEImageHandlerAndroid();

        bool initWithBitmap(JNIEnv *env, jobject bitmap, bool enableReversion = false);

        jobject getResultBitmap(JNIEnv *env);

        jobject getResultBitmap(JNIEnv *env, int resultWidth, int resultHeight);

        void processingFilters();

        void swapBufferFBO();


        void setRenderSize(int newRenderWidth, int newRenderHeight) {
            renderWidth = newRenderWidth;
            renderHeight = newRenderHeight;
        };
        int getRenderWidth() { return renderWidth; };
        int getRenderHeight() { return renderHeight; };


        void setSourceSize(int newSourceWidth, int newSourceHeight) {
            sourceWidth = newSourceWidth;
            sourceHeight = newSourceHeight;
        };
        int getSourceWidth() { return sourceWidth; };
        int getSourceHeight() { return sourceHeight; };


        void setCrop(int newCropXPos, int newCropYPos, int newCropWidth, int newCropHeight) {
            cropXPos = newCropXPos;
            cropYPos = newCropYPos;
            cropWidth = newCropWidth;
            cropHeight = newCropHeight;
        };
        int getCropXPos() { return cropXPos; };
        int getCropYPos() { return cropYPos; };
        int getCropWidth() {
            if (cropWidth > 0) return cropWidth;
            return sourceWidth;
        };
        int getCropHeight() {
            if (cropHeight > 0) return cropHeight;
            return sourceHeight;
        };


        void setCenter(float newCenterX, float newCenterY) {
            centerX = newCenterX;
            centerY = newCenterY;
        };
        int getCenterX() { return centerX; };
        int getCenterY() { return centerY; };

        void setOutputScale(float newOutputScale) {
            outputScale = newOutputScale;
        };
        float getOutputScale() { return outputScale; };


        void setOutputSize(int newOutputWidth, int newOutputHeight) {
            outputWidth = newOutputWidth;
            outputHeight = newOutputHeight;
        };
        int getOutputWidth() { return outputWidth; };
        int getOutputHeight() { return outputHeight; };


        void setCropScale(float newCropScale) {
            cropScale = newCropScale;
        };
        float getCropScale() { return cropScale; };


        void setFocusType(int newFocusType) {
            focusType = newFocusType;
        }
        void setFocusPosition(float newFocusXPosition, float newFocusYPosition) {
            focusXPosition = newFocusXPosition;
            focusYPosition = newFocusYPosition;
        }
        void setFocusRadius(float newFocusRadius) {
            focusRadius = newFocusRadius;
        }
        int getFocusType() { return focusType;};
        float getFocusYPosition() { return focusYPosition;};
        float getFocusXPosition() { return focusXPosition;};
        float getFocusRadius() { return focusRadius;};

        void release(JNIEnv *env);

    private:
        JniBitmap *storeBitmap(JNIEnv *env, jobject bitmap);

        jobject restoreBitmap(JNIEnv *env, JniBitmap *jniBitmap);

        JniBitmap *scaleBitmap(JNIEnv *env, JniBitmap *jniBitmap, uint32_t width, uint32_t height);

        JniBitmap *rotateBitmap(JNIEnv *env, JniBitmap *jniBitmap, int amountOf90DegreeRotations);

    };
}

#endif