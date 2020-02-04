/*
* cgeFrameRenderer.cpp
*
*  Created on: 2015-11-25
*      Author: Wang Yang
*        Mail: admin@wysaid.org
*/

#include "cgeFrameRenderer.h"
#include "cgeMultipleEffects.h"
#include "cgeUtilFunctions.h"

#include <typeinfo>

#include <android/log.h>
#include <chrono>

#if defined(DEBUG) || defined(_DEBUG)

#define CHECK_RENDERER_STATUS \
if(m_frameHandler == nullptr)\
{\
    CGE_LOG_ERROR("CGEFrameRenderer is not initialized!!\n");\
    return;\
}

#else

#define CHECK_RENDERER_STATUS

#endif

namespace CGE {
    void CGEFastFrameHandler::processingFilters() {
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
        // glFlush();
    }

    void CGEFastFrameHandler::swapBufferFBO() {
        useImageFBO();
        std::swap(m_bufferTextures[0], m_bufferTextures[1]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                               m_bufferTextures[0], 0);

        //为了效率， 高帧率绘制期间不检测错误.
    }


    //////////////////////////////////////////////

    CGEFrameRenderer::CGEFrameRenderer() : m_textureDrawer(nullptr), m_textureDrawerExtOES(nullptr),
                                           m_cacheDrawer(nullptr), m_isUsingMask(false),
                                           m_drawerFlipScaleX(1.0f), m_drawerFlipScaleY(1.0f),
                                           m_frameHandler(nullptr) {

    }


    CGEFrameRenderer::~CGEFrameRenderer() {
        CGE_LOG_INFO("CGEFrameRenderer::~CGEFrameRenderer");

        delete m_textureDrawer;
        delete m_textureDrawerExtOES;

        delete m_cacheDrawer;
        m_cacheDrawer = nullptr;

        delete m_frameHandler;
        m_frameHandler = nullptr;
        CGE_LOG_INFO("CGEFrameRenderer::~CGEFrameRenderer()");
    }

    bool CGEFrameRenderer::init(int srcWidth, int srcHeight, int dstWidth, int dstHeight) {
        CGE_LOG_INFO("CGEFrameRenderer::init...");
        m_srcSize.set(srcWidth, srcHeight);
        m_dstSize.set(dstWidth, dstHeight);

        setSourceSize(srcWidth, srcHeight);

        //有可能通过预先设置为 mask版
        if (m_textureDrawer == nullptr)
            m_textureDrawer = TextureDrawer::create();

        if (m_textureDrawerExtOES == nullptr)
            m_textureDrawerExtOES = TextureDrawer4ExtOES::create();

        CGE_LOG_INFO("create cache texture: %d x %d", m_dstSize.width, m_dstSize.height);

        if (m_cacheDrawer == nullptr)
            m_cacheDrawer = TextureDrawer::create();

        _calcViewport(srcWidth, srcHeight, dstWidth, dstHeight);

        if (m_frameHandler == nullptr) {
            m_frameHandler = new CGEFastFrameHandler();
        }

        return m_textureDrawer != nullptr && m_textureDrawerExtOES != nullptr &&
               m_frameHandler->initWithRawBufferData(nullptr, dstWidth, dstHeight,
                                                     CGE_FORMAT_RGBA_INT8, false);
    }

    void CGEFrameRenderer::srcResize(int width, int height) {
        CHECK_RENDERER_STATUS;

        CGE_LOG_INFO("srcResize: %d, %d", width, height);
        CGESizei sz = m_frameHandler->getOutputFBOSize();
        if (sz.width <= 0 || sz.height <= 0) {
            CGE_LOG_ERROR("CGEFrameRenderer not initialized!\n");
            return;
        }
        _calcViewport(width, height, sz.width, sz.height);
    }

    void CGEFrameRenderer::_calcViewport(int srcWidth, int srcHeight, int dstWidth, int dstHeight) {
        float scaling = CGE_MAX(dstWidth / (float) srcWidth, dstHeight / (float) srcHeight);

        if (scaling != 0.0f) {
            float sw = srcWidth * scaling, sh = srcHeight * scaling;
            m_viewport[0] = (dstWidth - sw) / 2.0f;
            m_viewport[1] = (dstHeight - sh) / 2.0f;
            m_viewport[2] = sw;
            m_viewport[3] = sh;
            CGE_LOG_INFO("CGEFrameRenderer - viewport: %d, %d, %d, %d", m_viewport[0],
                         m_viewport[1], m_viewport[2], m_viewport[3]);
        }
    }

    void CGEFrameRenderer::update(GLuint externalTexture, float *transformMatrix) {
        CHECK_RENDERER_STATUS;

        m_frameHandler->useImageFBO();
        glViewport(m_viewport[0], m_viewport[1], m_viewport[2], m_viewport[3]);

        m_textureDrawerExtOES->setTransform(transformMatrix);
        m_textureDrawerExtOES->drawTexture(externalTexture);
    }

    void CGEFrameRenderer::runProc() {
        CHECK_RENDERER_STATUS;
        m_resultMutex.lock();
        m_frameHandler->processingFilters();
        m_resultMutex.unlock();
    }

    void CGEFrameRenderer::render(int x, int y, int width, int height) {
        CHECK_RENDERER_STATUS;

        glViewport(x, y, width, height);
        m_textureDrawer->drawTexture(m_frameHandler->getTargetTextureID());
    }

    void CGEFrameRenderer::drawCache() {
        CHECK_RENDERER_STATUS;

        m_cacheDrawer->drawTexture(m_frameHandler->getTargetTextureID());
    }

    void CGEFrameRenderer::setSrcRotation(float rad) {
        m_textureDrawerExtOES->setRotation(rad);
    }

    void CGEFrameRenderer::setSrcFlipScale(float x, float y) {
        m_textureDrawerExtOES->setFlipScale(x, y);
    }

    void CGEFrameRenderer::setRenderRotation(float rad) {
        m_textureDrawer->setRotation(rad);
    }

    void CGEFrameRenderer::setRenderFlipScale(float x, float y) {
        m_drawerFlipScaleX = x;
        m_drawerFlipScaleY = y;
        m_textureDrawer->setFlipScale(x, y);
    }

    void CGEFrameRenderer::setFilterWithConfig(CGEConstString config, CGETextureLoadFun texLoadFunc,
                                               jboolean shouldClearOlder, void *loadArg) {
        CHECK_RENDERER_STATUS;

        std::unique_lock<std::mutex> uniqueLock(m_resultMutex);

        if (config == nullptr || *config == '\0') {
            m_frameHandler->clearImageFilters(true);
            return;
        }

        CGEMutipleEffectFilter *filter = new CGEMutipleEffectFilter;
        filter->setTextureLoadFunction(texLoadFunc, loadArg);

        if (!filter->initWithEffectString(nullptr, config)) {
            delete filter;
        } else {
            if (shouldClearOlder) m_frameHandler->clearImageFilters(true);

            m_frameHandler->addImageFilter(filter);
        }
    }

    void CGEFrameRenderer::setFilter(CGEImageFilterInterfaceAbstract *filter) {
        CHECK_RENDERER_STATUS;

        std::unique_lock<std::mutex> uniqueLock(m_resultMutex);
        m_frameHandler->clearImageFilters(true);
        m_frameHandler->addImageFilter(filter);
    }

    void CGEFrameRenderer::setFilterIntensity(int progress) {
        CHECK_RENDERER_STATUS;

        m_resultMutex.lock();
        auto &filters = m_frameHandler->peekFilters();
        for (auto &filter : filters) {
            filter->setIntensity(progress);
        }
        m_resultMutex.unlock();
    }

    void CGEFrameRenderer::setFilterValueAtIndex(JNIEnv *env, const char *value, int index) {
        CHECK_RENDERER_STATUS;

        m_resultMutex.lock();
        auto &filters = m_frameHandler->peekFilters();

        if (index < 0 || filters.empty()) {
            return;
        }

        CGEImageFilterInterfaceAbstract *filter = nullptr;

        if (index < filters.size()) {
            filter = filters[index];
        } else {
            return;
        }

        assert(filter != nullptr); //impossible

        CGEMutipleEffectFilter *multiFilter = dynamic_cast<CGEMutipleEffectFilter *>(filter);

        // Warmth
        if (CGEWarmthFilter *warmthFilter = dynamic_cast<CGEWarmthFilter *>(multiFilter->getFilter())) {
            warmthFilter->setWarmth(atof(value));
        }
            // Vignette
        else if (CGEVignetteFilter *vignetteFilter = dynamic_cast<CGEVignetteFilter *>(multiFilter->getFilter())) {
            vignetteFilter->setVignetteStrength(atof(value));
        }
            // Saturation
        else if (CGESaturationFilter *saturationFilter = dynamic_cast<CGESaturationFilter *>(multiFilter->getFilter())) {
            saturationFilter->setSaturation(atof(value));
        }
            // Contrast
        else if (CGEContrastFilter *contrastFilter = dynamic_cast<CGEContrastFilter *>(multiFilter->getFilter())) {
            contrastFilter->setContrast(atof(value));
        }
            // Exposure
        else if (CGEExposureFilter *exposureFilter = dynamic_cast<CGEExposureFilter *>(multiFilter->getFilter())) {
            exposureFilter->setExposure(atof(value));
        }
            // Shadows
        else if (CGEShadowsFilter *shadowsFilter = dynamic_cast<CGEShadowsFilter *>(multiFilter->getFilter())) {
            shadowsFilter->setShadows(atof(value));
        }
            // Highlights
        else if (CGEHighlightsFilter *highlightsFilter = dynamic_cast<CGEHighlightsFilter *>(multiFilter->getFilter())) {
            highlightsFilter->setHighlights(atof(value));
        }
            // Sharpen
        else if (CGESharpenFilter *sharpenFilter = dynamic_cast<CGESharpenFilter *>(multiFilter->getFilter())) {
            sharpenFilter->setRenderSize(getSourceWidth(), getSourceHeight());
            sharpenFilter->setSharpness(atof(value));
        }
            // Rotation
        else if (CGERotationFilter *rotationFilter = dynamic_cast<CGERotationFilter *>(multiFilter->getFilter())) {
            float a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p;
            if (sscanf(value,
                       "%f%*c%f%*c%f%*c%f%*c%f%*c%f%*c%f%*c%f%*c%f%*c%f%*c%f%*c%f%*c%f%*c%f%*c%f%*c%f",
                       &a, &b, &c, &d, &e, &f, &g, &h, &i, &j, &k, &l, &m, &n, &o, &p) != 16) {
                return;
            }
            float matrix[16] = {a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p};

            // transformationFilter->setOrthographicMatrix(getRenderWidth(), getRenderHeight());
            rotationFilter->setTransformationMatrix(matrix);
        }
            // Transformation
        else if (CGETransformationFilter *transformationFilter = dynamic_cast<CGETransformationFilter *>(multiFilter->getFilter())) {
            float m[16];
            float xPos, yPos, cropWidth, cropHeight, sourceWidth, sourceHeight;

            int amountOfArguments = sscanf(value,
                                           "%f%*c%f%*c%f%*c%f%*c%f%*c%f%*c%f%*c%f%*c%f%*c%f%*c%f%*c%f%*c%f%*c%f%*c%f%*c%f%*c%f%*c%f%*c%f%*c%f%*c%f%*c%f",
                                           m + 0, m + 1, m + 2, m + 3, m + 4, m + 5, m + 6, m + 7,
                                           m + 8, m + 9, m + 10, m + 11, m + 12, m + 13, m + 14,
                                           m + 15,
                                           &xPos, &yPos, &cropWidth, &cropHeight, &sourceWidth,
                                           &sourceHeight);

            if (amountOfArguments == 20) {
                transformationFilter->setTransformationMatrix(m);
                transformationFilter->setCrop(xPos, yPos, cropWidth, cropHeight);
            } else if (amountOfArguments == 16) {
                transformationFilter->setTransformationMatrix(m);
            } else {
                amountOfArguments = sscanf(value, "%f%*c%f%*c%f%*c%f",
                                           &xPos, &yPos, &cropWidth, &cropHeight);

                if (amountOfArguments == 4) {
                    transformationFilter->setCrop(xPos, yPos, cropWidth, cropHeight);
                } else {
                    CGE_LOG_ERROR("adjusting transformation failed: %s", value);
                    return;
                }
            }
        }
            // LUT
        else if (CGELookupFilter *lookupFilter = dynamic_cast<CGELookupFilter *>(multiFilter->getFilter())) {
            CGETexLoadArg texLoadArg;

            texLoadArg.env = env;
            texLoadArg.cls = env->FindClass("org/wysaid/nativePort/CGENativeLibrary");

            multiFilter->setTextureLoadFunction(cgeGlobalTextureLoadFunc, &texLoadArg);

            if (strcmp(value, "original.png") == 0) {
                lookupFilter->doNotUse(true);
            } else {
                lookupFilter->doNotUse(false);
                lookupFilter->setLookupTexture(multiFilter->loadResources(value));
            }
        }
            // shouldn't reach this
        else {
            filter->setIntensity(atof(value));
        }

        m_resultMutex.unlock();
    }

    void CGEFrameRenderer::setMaskTexture(GLuint maskTexture, float aspectRatio) {
        if (maskTexture == 0) {
            if (m_isUsingMask || m_textureDrawer == nullptr) {
                m_isUsingMask = false;
                delete m_textureDrawer;
                m_textureDrawer = TextureDrawer::create();
                m_textureDrawer->setFlipScale(m_drawerFlipScaleX, m_drawerFlipScaleY);
            }
            return;
        }

        m_isUsingMask = true;

        TextureDrawerWithMask *drawer = TextureDrawerWithMask::create();
        if (drawer == nullptr) {
            CGE_LOG_ERROR("init drawer program failed!");
            return;
        }

        delete m_textureDrawer;
        m_textureDrawer = drawer;
        drawer->setMaskTexture(maskTexture);
        setMaskTextureRatio(aspectRatio);
    }

    void CGEFrameRenderer::setMaskTextureRatio(float aspectRatio) {
        float dstRatio = m_dstSize.width / (float) m_dstSize.height;
        float s = dstRatio / aspectRatio;
        if (s > 1.0f) {
            m_textureDrawer->setFlipScale(m_drawerFlipScaleX / s, m_drawerFlipScaleY);
        } else {
            m_textureDrawer->setFlipScale(m_drawerFlipScaleX, s * m_drawerFlipScaleY);
        }
    }

    GLuint CGEFrameRenderer::getTargetTexture() {
        if (m_frameHandler == nullptr) {
            CGE_LOG_ERROR("CGEFrameRenderer is not initialized!!\n");
            return 0;
        }

        return m_frameHandler->getTargetTextureID();
    }

    GLuint CGEFrameRenderer::getBufferTexture() {
        if (m_frameHandler == nullptr) {
            CGE_LOG_ERROR("CGEFrameRenderer is not initialized!!\n");
            return 0;
        }

        return m_frameHandler->getBufferTextureID();
    }

    void CGEFrameRenderer::setMaskRotation(float rad) {
        if (!m_isUsingMask || m_textureDrawer == nullptr)
            return;

        TextureDrawerWithMask *drawer = dynamic_cast<TextureDrawerWithMask *>(m_textureDrawer);
        assert(drawer != nullptr);
        drawer->setMaskRotation(rad);
    }

    void CGEFrameRenderer::setMaskFlipScale(float x, float y) {
        if (!m_isUsingMask || m_textureDrawer == nullptr)
            return;

        TextureDrawerWithMask *drawer = dynamic_cast<TextureDrawerWithMask *>(m_textureDrawer);
        assert(drawer != nullptr);
        drawer->setMaskFlipScale(x, y);
    }

    void CGEFrameRenderer::setRenderSize(int newRenderWidth, int newRenderHeight) {
        renderWidth = newRenderWidth;
        renderHeight = newRenderHeight;
        CGE_LOG_INFO("render size is: %dx%d", renderWidth, renderHeight);
    }

}












