/*
 * cgeImageHandlerAndroid.cpp
 *
 *  Created on: 2015-12-20
 *      Author: Wang Yang
 *        Mail: admin@wysaid.org
 *	 Edited by: Niek Akerboom
 */

#include "cgeImageHandlerAndroid.h"
#include <android/bitmap.h>

#include <android/log.h>

namespace CGE {
    void CGEImageHandlerAndroid::processingFilters() {
        if (m_vecFilters.empty() || m_bufferTextures[0] == 0) {
            glFlush();
            return;
        }

        glDisable(GL_BLEND);
        assert(m_vertexArrayBuffer != 0);

        glViewport(0, 0, m_dstImageSize.width, m_dstImageSize.height);

        for (std::vector<CGEImageFilterInterfaceAbstract *>::iterator iter = m_vecFilters.begin();
             iter < m_vecFilters.end(); ++iter) {
            swapBufferFBO();
            glBindBuffer(GL_ARRAY_BUFFER, m_vertexArrayBuffer);
            (*iter)->render2Texture(this, m_bufferTextures[1], m_vertexArrayBuffer);

            glFlush();
        }

        glFinish();
    }

    void CGEImageHandlerAndroid::swapBufferFBO() {
        useImageFBO();
        std::swap(m_bufferTextures[0], m_bufferTextures[1]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                               m_bufferTextures[0], 0);
    }

    CGEImageHandlerAndroid::CGEImageHandlerAndroid() {
        CGE_LOG_INFO("CGEImageHandlerAndroid created!\n");

        setCenter(0.5, 0.5);
    }

    CGEImageHandlerAndroid::~CGEImageHandlerAndroid() {
        CGE_LOG_INFO("CGEImageHandlerAndroid released!\n");
    }

    bool CGEImageHandlerAndroid::initWithBitmap(JNIEnv *env, jobject bmp, bool enableReversion) {
        AndroidBitmapInfo info;

        int ret = AndroidBitmap_getInfo(env, bmp, &info);

        if (ret < 0) {
            CGE_LOG_ERROR("AndroidBitmap_getInfo() failed ! error=%d", ret);
            return false;
        }

        CGE_LOG_INFO(
                "color image :: width is %d; height is %d; stride is %d; format is %d;flags is %d",
                info.width, info.height, info.stride, info.format, info.flags);

        setSourceSize(info.width, info.height);

        if (info.format != ANDROID_BITMAP_FORMAT_RGBA_8888) {
            CGE_LOG_ERROR("Bitmap format is not RGBA_8888 !");
            return false;
        }

        char *row;

        ret = AndroidBitmap_lockPixels(env, bmp, (void **) &row);

        if (ret < 0) {
            CGE_LOG_ERROR("AndroidBitmap_lockPixels() failed ! error=%d", ret);
            return false;
        }

        bool flag = initWithRawBufferData(row, info.width, info.height, CGE_FORMAT_RGBA_INT8, enableReversion);

        AndroidBitmap_unlockPixels(env, bmp);

        return flag;
    }

    jobject CGEImageHandlerAndroid::getResultBitmap(JNIEnv *env) {
        jclass bitmapCls = env->FindClass("android/graphics/Bitmap");

        jmethodID createBitmapFunction = env->GetStaticMethodID(bitmapCls, "createBitmap",
                                                                "(IILandroid/graphics/Bitmap$Config;)Landroid/graphics/Bitmap;");
        jstring configName = env->NewStringUTF("ARGB_8888");
        jclass bitmapConfigClass = env->FindClass("android/graphics/Bitmap$Config");
        jmethodID valueOfBitmapConfigFunction = env->GetStaticMethodID(bitmapConfigClass, "valueOf",
                                                                       "(Ljava/lang/String;)Landroid/graphics/Bitmap$Config;");
        jobject bitmapConfig = env->CallStaticObjectMethod(bitmapConfigClass,
                                                           valueOfBitmapConfigFunction, configName);
        env->DeleteLocalRef(configName);
        jobject newBitmap = env->CallStaticObjectMethod(bitmapCls, createBitmapFunction,
                                                        getSourceWidth(), getSourceHeight(),
                                                        bitmapConfig);

        char *row;

        int ret = AndroidBitmap_lockPixels(env, newBitmap, (void **) &row);

        if (ret < 0) {
            CGE_LOG_ERROR("AndroidBitmap_lockPixels() failed ! error=%d", ret);
            return nullptr;
        }

        getOutputBufferData(row, CGE_FORMAT_RGBA_INT8);
        AndroidBitmap_unlockPixels(env, newBitmap);

        return newBitmap;
    }

    jobject CGEImageHandlerAndroid::getResultBitmap(JNIEnv *env, int resultWidth, int resultHeight) {
        JniBitmap *jniBitmap = storeBitmap(env, getResultBitmap(env));
        jniBitmap = scaleBitmap(env, jniBitmap, resultWidth, resultHeight);

        return restoreBitmap(env, jniBitmap);
    }

    void CGEImageHandlerAndroid::release(JNIEnv *env) {
        JniBitmap *jniBitmap = storeBitmap(env, getResultBitmap(env));
        if (jniBitmap->_storedBitmapPixels == NULL) return;

        delete[] jniBitmap->_storedBitmapPixels;
        jniBitmap->_storedBitmapPixels = NULL;

        delete jniBitmap;
    }

    JniBitmap *CGEImageHandlerAndroid::storeBitmap(JNIEnv *env, jobject bitmap) {
        AndroidBitmapInfo bitmapInfo;
        uint32_t *storedBitmapPixels = NULL;
        void *bitmapPixels;

        int ret;
        if ((ret = AndroidBitmap_getInfo(env, bitmap, &bitmapInfo)) < 0) {
            CGE_LOG_ERROR("AndroidBitmap_getInfo() failed ! error=%d", ret);
            return NULL;
        } else if (bitmapInfo.format != ANDROID_BITMAP_FORMAT_RGBA_8888) {
            CGE_LOG_ERROR("Bitmap format is not RGBA_8888!");
            return NULL;
        } else if ((ret = AndroidBitmap_lockPixels(env, bitmap, &bitmapPixels)) < 0) {
            CGE_LOG_ERROR("AndroidBitmap_lockPixels() failed ! error=%d", ret);
            return NULL;
        }

        uint32_t *src = (uint32_t *) bitmapPixels;
        storedBitmapPixels = new uint32_t[bitmapInfo.height * bitmapInfo.width];
        int pixelsCount = bitmapInfo.height * bitmapInfo.width;
        memcpy(storedBitmapPixels, src, sizeof(uint32_t) * pixelsCount);

        AndroidBitmap_unlockPixels(env, bitmap);

        JniBitmap *jniBitmap = new JniBitmap();
        jniBitmap->_bitmapInfo = bitmapInfo;
        jniBitmap->_storedBitmapPixels = storedBitmapPixels;

        return jniBitmap;
    }

    jobject CGEImageHandlerAndroid::restoreBitmap(JNIEnv *env, JniBitmap *jniBitmap) {
        if (jniBitmap->_storedBitmapPixels == NULL) {
            CGE_LOG_ERROR("no bitmap data was stored. returning null...");
            return NULL;
        }

        jclass bitmapCls = env->FindClass("android/graphics/Bitmap");
        jmethodID createBitmapFunction = env->GetStaticMethodID(bitmapCls, "createBitmap",
                                                                "(IILandroid/graphics/Bitmap$Config;)Landroid/graphics/Bitmap;");
        jstring configName = env->NewStringUTF("ARGB_8888");
        jclass bitmapConfigClass = env->FindClass("android/graphics/Bitmap$Config");
        jmethodID valueOfBitmapConfigFunction = env->GetStaticMethodID(bitmapConfigClass, "valueOf",
                                                                       "(Ljava/lang/String;)Landroid/graphics/Bitmap$Config;");

        jobject bitmapConfig = env->CallStaticObjectMethod(bitmapConfigClass,
                                                           valueOfBitmapConfigFunction, configName);
        jobject newBitmap = env->CallStaticObjectMethod(bitmapCls, createBitmapFunction,
                                                        jniBitmap->_bitmapInfo.width,
                                                        jniBitmap->_bitmapInfo.height,
                                                        bitmapConfig);

        int ret;
        void *bitmapPixels;
        if ((ret = AndroidBitmap_lockPixels(env, newBitmap, &bitmapPixels)) < 0) {
            CGE_LOG_ERROR("AndroidBitmap_lockPixels() failed ! error=%d", ret);
            return NULL;
        }

        uint32_t *newBitmapPixels = (uint32_t *) bitmapPixels;
        int pixelsCount = jniBitmap->_bitmapInfo.height * jniBitmap->_bitmapInfo.width;
        memcpy(newBitmapPixels, jniBitmap->_storedBitmapPixels, sizeof(uint32_t) * pixelsCount);
        AndroidBitmap_unlockPixels(env, newBitmap);

        return newBitmap;
    }

    typedef struct {
        uint8_t alpha, red, green, blue;
    } ARGB;

    int32_t convertArgbToInt(ARGB argb) {
        return (argb.alpha) | (argb.red << 24) | (argb.green << 16) | (argb.blue << 8);
    }

    void convertIntToArgb(uint32_t pixel, ARGB *argb) {
        argb->red = ((pixel >> 24) & 0xff);
        argb->green = ((pixel >> 16) & 0xff);
        argb->blue = ((pixel >> 8) & 0xff);
        argb->alpha = (pixel & 0xff);
    }

    JniBitmap *
    CGEImageHandlerAndroid::scaleBitmap(JNIEnv *env, JniBitmap *jniBitmap, uint32_t newWidth,
                                        uint32_t newHeight) {
        if (jniBitmap->_storedBitmapPixels == NULL) return NULL;

        uint32_t oldWidth = jniBitmap->_bitmapInfo.width;
        uint32_t oldHeight = jniBitmap->_bitmapInfo.height;
        uint32_t *previousData = jniBitmap->_storedBitmapPixels;
        uint32_t *newBitmapPixels = new uint32_t[newWidth * newHeight];

        // position of the top left pixel of the 4 pixels to use interpolation on
        int xTopLeft, yTopLeft;
        int x, y, lastTopLefty;
        float xRatio = (float) newWidth / (float) oldWidth, yratio =
                (float) newHeight / (float) oldHeight;

        // Y color ratio to use on left and right pixels for interpolation
        float ycRatio2 = 0, ycRatio1 = 0;

        // pixel target in the src
        float xt, yt;

        // X color ratio to use on left and right pixels for interpolation
        float xcRatio2 = 0, xcratio1 = 0;

        ARGB rgbTopLeft, rgbTopRight, rgbBottomLeft, rgbBottomRight, rgbTopMiddle, rgbBottomMiddle, result;
        for (x = 0; x < newWidth; ++x) {
            xTopLeft = (int) (xt = x / xRatio);

            // when meeting the most right edge, move left a little
            if (xTopLeft >= oldWidth - 1) xTopLeft--;

            if (xt <= xTopLeft + 1) {
                // we are between the left and right pixel
                xcratio1 = xt - xTopLeft;
                // color ratio in favor of the right pixel color
                xcRatio2 = 1 - xcratio1;
            }

            for (y = 0, lastTopLefty = -30000; y < newHeight; ++y) {
                yTopLeft = (int) (yt = y / yratio);

                // when meeting the most bottom edge, move up a little
                if (yTopLeft >= oldHeight - 1)--yTopLeft;

                if (lastTopLefty == yTopLeft - 1) {
                    // we went down only one rectangle
                    rgbTopLeft = rgbBottomLeft;
                    rgbTopRight = rgbBottomRight;
                    rgbTopMiddle = rgbBottomMiddle;

                    convertIntToArgb(previousData[((yTopLeft + 1) * oldWidth) + xTopLeft],
                                     &rgbBottomLeft);
                    convertIntToArgb(previousData[((yTopLeft + 1) * oldWidth) + (xTopLeft + 1)],
                                     &rgbBottomRight);
                    rgbBottomMiddle.alpha = rgbBottomLeft.alpha * xcRatio2
                                            + rgbBottomRight.alpha * xcratio1;
                    rgbBottomMiddle.red = rgbBottomLeft.red * xcRatio2
                                          + rgbBottomRight.red * xcratio1;
                    rgbBottomMiddle.green = rgbBottomLeft.green * xcRatio2
                                            + rgbBottomRight.green * xcratio1;
                    rgbBottomMiddle.blue = rgbBottomLeft.blue * xcRatio2
                                           + rgbBottomRight.blue * xcratio1;
                } else if (lastTopLefty != yTopLeft) {
                    // we went to a totally different rectangle (happens in every loop start,and might happen more when making the picture smaller)
                    convertIntToArgb(previousData[(yTopLeft * oldWidth) + xTopLeft], &rgbTopLeft);

                    convertIntToArgb(previousData[((yTopLeft + 1) * oldWidth) + xTopLeft],
                                     &rgbTopRight);
                    rgbTopMiddle.alpha = rgbTopLeft.alpha * xcRatio2
                                         + rgbTopRight.alpha * xcratio1;
                    rgbTopMiddle.red = rgbTopLeft.red * xcRatio2
                                       + rgbTopRight.red * xcratio1;
                    rgbTopMiddle.green = rgbTopLeft.green * xcRatio2
                                         + rgbTopRight.green * xcratio1;
                    rgbTopMiddle.blue = rgbTopLeft.blue * xcRatio2
                                        + rgbTopRight.blue * xcratio1;

                    convertIntToArgb(previousData[((yTopLeft + 1) * oldWidth) + xTopLeft],
                                     &rgbBottomLeft);

                    convertIntToArgb(previousData[((yTopLeft + 1) * oldWidth) + (xTopLeft + 1)],
                                     &rgbBottomRight);
                    rgbBottomMiddle.alpha = rgbBottomLeft.alpha * xcRatio2
                                            + rgbBottomRight.alpha * xcratio1;
                    rgbBottomMiddle.red = rgbBottomLeft.red * xcRatio2
                                          + rgbBottomRight.red * xcratio1;
                    rgbBottomMiddle.green = rgbBottomLeft.green * xcRatio2
                                            + rgbBottomRight.green * xcratio1;
                    rgbBottomMiddle.blue = rgbBottomLeft.blue * xcRatio2
                                           + rgbBottomRight.blue * xcratio1;
                }

                lastTopLefty = yTopLeft;

                if (yt <= yTopLeft + 1) {
                    // color ratio in favor of the bottom pixel color
                    ycRatio1 = yt - yTopLeft;
                    ycRatio2 = 1 - ycRatio1;
                }

                // prepared all pixels to look at, so finally set the new pixel data
                result.alpha = rgbTopMiddle.alpha * ycRatio2
                               + rgbBottomMiddle.alpha * ycRatio1;
                result.blue = rgbTopMiddle.blue * ycRatio2
                              + rgbBottomMiddle.blue * ycRatio1;
                result.red = rgbTopMiddle.red * ycRatio2
                             + rgbBottomMiddle.red * ycRatio1;
                result.green = rgbTopMiddle.green * ycRatio2
                               + rgbBottomMiddle.green * ycRatio1;
                newBitmapPixels[(y * newWidth) + x] = convertArgbToInt(result);
            }
        }

        //get rid of old data, and replace it with new one
        delete[] previousData;

        jniBitmap->_storedBitmapPixels = newBitmapPixels;
        jniBitmap->_bitmapInfo.width = newWidth;
        jniBitmap->_bitmapInfo.height = newHeight;

        return jniBitmap;
    }

    JniBitmap *CGEImageHandlerAndroid::rotateBitmap(JNIEnv *env, JniBitmap *jniBitmap,
                                                    int amountOf90DegreeRotations) {
        if (jniBitmap->_storedBitmapPixels == NULL) return NULL;

        amountOf90DegreeRotations = amountOf90DegreeRotations % 4;

        if (amountOf90DegreeRotations == 1) {
            uint32_t *previousData = jniBitmap->_storedBitmapPixels;
            uint32_t newWidth = jniBitmap->_bitmapInfo.height;
            uint32_t newHeight = jniBitmap->_bitmapInfo.width;
            jniBitmap->_bitmapInfo.width = newWidth;
            jniBitmap->_bitmapInfo.height = newHeight;
            uint32_t *newBitmapPixels = new uint32_t[newWidth * newHeight];
            int whereToGet = 0;

            for (int x = newWidth - 1; x >= 0; --x) {
                for (int y = 0; y < newHeight; ++y) {
                    uint32_t pixel = previousData[whereToGet++];
                    newBitmapPixels[newWidth * y + x] = pixel;
                }
            }

            delete[] previousData;
            jniBitmap->_storedBitmapPixels = newBitmapPixels;
        } else if (amountOf90DegreeRotations == 2) {
            uint32_t *pixels = jniBitmap->_storedBitmapPixels;
            uint32_t *pixels2 = jniBitmap->_storedBitmapPixels;
            uint32_t width = jniBitmap->_bitmapInfo.width;
            uint32_t height = jniBitmap->_bitmapInfo.height;
            int whereToGet = 0;

            for (int y = height - 1; y >= height / 2; --y) {
                for (int x = width - 1; x >= 0; --x) {
                    uint32_t tempPixel = pixels2[width * y + x];
                    pixels2[width * y + x] = pixels[whereToGet];
                    pixels[whereToGet] = tempPixel;
                    ++whereToGet;
                }
            }

            if (height % 2 == 1) {
                int y = height / 2;
                whereToGet = width * y;
                int lastXToHandle = width % 2 == 0 ? (width / 2) : (width / 2) - 1;
                for (int x = width - 1; x >= lastXToHandle; --x) {
                    uint32_t tempPixel = pixels2[width * y + x];
                    pixels2[width * y + x] = pixels[whereToGet];
                    pixels[whereToGet] = tempPixel;
                    ++whereToGet;
                }
            }
        } else if (amountOf90DegreeRotations == 3) {
            uint32_t *previousData = jniBitmap->_storedBitmapPixels;
            uint32_t newWidth = jniBitmap->_bitmapInfo.height;
            uint32_t newHeight = jniBitmap->_bitmapInfo.width;
            jniBitmap->_bitmapInfo.width = newWidth;
            jniBitmap->_bitmapInfo.height = newHeight;
            uint32_t *newBitmapPixels = new uint32_t[newWidth * newHeight];
            int whereToGet = 0;

            for (int x = 0; x < newWidth; ++x) {
                for (int y = newHeight - 1; y >= 0; --y) {
                    uint32_t pixel = previousData[whereToGet++];
                    newBitmapPixels[newWidth * y + x] = pixel;
                }
            }

            delete[] previousData;
            jniBitmap->_storedBitmapPixels = newBitmapPixels;
        }

        return jniBitmap;
    }
}
