/*
 * cgeMultipleEffects.cpp
 *
 *  Created on: 2013-12-13
 *      Author: Wang Yang
 *        Mail: admin@wysaid.org
 *	 Edited by: Niek Akerboom
*/

#include "cgeMultipleEffects.h"
#include "cgeDataParsingEngine.h"
#include <cmath>
#include <cctype>

#include <android/log.h>

static CGEConstString s_fshHTFilterMix = CGE_SHADER_STRING_PRECISION_M (
    varying vec2 textureCoordinate;
    uniform sampler2D inputImageTexture;
    uniform sampler2D originImageTexture;
    uniform float intensity;

    void main() {
        vec4 src = texture2D(inputImageTexture, textureCoordinate);
        vec4 origin = texture2D(originImageTexture, textureCoordinate);
        gl_FragColor = mix(origin, src, intensity);
    }
);

namespace CGE {
    CGEConstString CGEMutipleMixFilter::paramIntensityName = "intensity";
    CGEConstString CGEMutipleMixFilter::paramOriginImageName = "originImageTexture";

    bool CGEMutipleMixFilter::init() {
        if (initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshHTFilterMix)) {
            m_program.bind();
            m_program.sendUniformi(paramOriginImageName, 1);
            setIntensity(1.0f);

            return true;
        }

        return false;
    }

    void CGEMutipleMixFilter::setIntensity(float value) {
        m_intensity = value;
        m_program.bind();
        m_program.sendUniformf(paramIntensityName, m_intensity);
    }

    void CGEMutipleMixFilter::render2Texture(CGEImageHandlerInterface *handler, GLuint srcTexture, GLuint vertexBufferID) {
        handler->setAsTarget();
        m_program.bind();

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, handler->getBufferTextureID());

        if (m_uniformParam != nullptr)
            m_uniformParam->assignUniforms(handler, m_program.programID());

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, srcTexture);

        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        cgeCheckGLError("glDrawArrays");
    }

    bool CGEMutipleMixFilter::needToMix() {
        return fabsf(m_intensity - 1.0f) > 0.01f;
    }

    bool CGEMutipleMixFilter::noIntensity() {
        return fabsf(m_intensity) < 0.01f;
    }

    //////////////////////////////////////////////////////////////////////////

    CGEMutipleEffectFilter::CGEMutipleEffectFilter() : m_loadFunc(nullptr), m_unloadFunc(nullptr),
                                                       m_loadParam(nullptr), m_unloadParam(nullptr),
                                                       m_texCache(0), m_isWrapper(false),
                                                       m_texLoadFunc(nullptr),
                                                       m_texLoadParam(nullptr) {
        // empty constructor
    }

    CGEMutipleEffectFilter::~CGEMutipleEffectFilter() {
        clearFilters();
        glDeleteTextures(1, &m_texCache);
        CGE_LOG_INFO("CGEMutipleEffectFilter Release...\n");
    }

    void CGEMutipleEffectFilter::clearFilters() {
        for (std::vector<CGEImageFilterInterface *>::iterator iter = m_vecFilters.begin();
             iter != m_vecFilters.end(); ++iter) {
            delete *iter;
        }

        m_vecFilters.clear();
    }

    void CGEMutipleEffectFilter::setBufferLoadFunction(CGEBufferLoadFun fLoad, void *loadParam,
                                                       CGEBufferUnloadFun fUnload,
                                                       void *unloadParam) {
        m_loadFunc = fLoad;
        m_loadParam = loadParam;
        m_unloadFunc = fUnload;
        m_unloadParam = unloadParam;
    }

    void CGEMutipleEffectFilter::setTextureLoadFunction(CGETextureLoadFun texLoader, void *arg) {
        m_texLoadFunc = texLoader;
        m_texLoadParam = arg;
    }

    GLuint CGEMutipleEffectFilter::loadResources(const char *textureName, int *width, int *height) {
        int w, h;

        if (m_texLoadFunc != nullptr) {
            GLuint tex = m_texLoadFunc(textureName, &w, &h, m_texLoadParam);

            if (tex != 0) {
                if (width != nullptr) *width = w;
                if (height != nullptr) *height = h;

                return tex;
            }
        }

        void *bufferData = nullptr;
        CGEBufferLoadFun loadFunc = m_loadFunc;
        CGEBufferUnloadFun unloadFunc = m_unloadFunc;
        void *loadArg = m_loadParam;
        void *unloadArg = m_unloadParam;

        if (loadFunc == nullptr) {
            loadFunc = cgeGetCommonLoadFunc();
            loadArg = cgeGetCommonLoadArg();
            unloadFunc = cgeGetCommonUnloadFunc();
            unloadArg = cgeGetCommonUnloadArg();
        }

        CGEBufferFormat fmt;
        void *pArg;

        if (loadFunc == nullptr || (pArg = loadFunc(textureName, &bufferData, &w, &h, &fmt, loadArg)) == nullptr) {
            CGE_LOG_ERROR("Load texture %s failed!\n", textureName);
            return 0;
        }

        GLenum dataFmt, channelFmt;
        cgeGetDataAndChannelByFormat(fmt, &dataFmt, &channelFmt, nullptr);
        GLuint texture = cgeGenTextureWithBuffer(bufferData, w, h, channelFmt, dataFmt);

        if (width != nullptr) *width = w;
        if (height != nullptr) *height = h;
        if (unloadFunc != nullptr) unloadFunc(pArg, unloadArg);

        return texture;
    }

    bool CGEMutipleEffectFilter::initCustomize() {
        return m_mixFilter.init();
    }

    bool CGEMutipleEffectFilter::initWithEffectString(CGEImageHandlerAndroid *handler, const char *effectString) {
        const char *ptr = effectString;

        if (ptr == nullptr || *ptr == '\0' || strncmp(ptr, "@unavailable", 12) == 0) return false;

        //////////////////////////////////////////////////////////////////////////
        //Hardcode for parsing the common effects.
        //////////////////////////////////////////////////////////////////////////

        char buffer[128];

        m_isWrapper = false;

        if (*ptr == '#') {
            char *pBuffer = buffer;
            ++ptr;

            while (*ptr != '\0' && !isspace(*ptr) && (pBuffer - buffer) < sizeof(buffer)) {
                *pBuffer++ = *ptr++;
            }

            *pBuffer = '\0';

            if (strcmp(buffer, "unpack") == 0) {
                m_isWrapper = true;
            }
        }

        if (!m_isWrapper && !m_mixFilter.init()) {
            return false;
        }

        while (*ptr != '\0') {
            while (*ptr != '\0' && *ptr != '@') ++ptr;
            while (*ptr != '\0' && (*ptr == '@' || *ptr == ' ' || *ptr == '\t')) ++ptr;

            if (*ptr == '\0') break;

            //2015-1-29 隐患fix, 将下一条指令直接取出再进行判断
            char *pBuffer = buffer;

            while (*ptr != '\0' && !isspace(*ptr) && (pBuffer - buffer) < sizeof(buffer)) {
                *pBuffer++ = *ptr++;
            }

            *pBuffer = '\0';

            if (strcmp(buffer, "adjust") == 0) {
                CGEDataParsingEngine::adjustParser(handler, ptr, this);
            } else {
                CGE_LOG_ERROR("指令未被解析(Invalid parameters):%s", ptr);
            }
        }

        if (m_vecFilters.empty()) {
            CGE_LOG_ERROR("特效指令 \"%s\" 无法生成任何特效!\n", effectString);
            return false;
        }

        return true;
    }

    void CGEMutipleEffectFilter::setIntensity(float value) {
        if (!m_isWrapper) m_mixFilter.setIntensity(value);
    }

    void
    CGEMutipleEffectFilter::render2Texture(CGEImageHandlerInterface *handler, GLuint srcTexture,
                                           GLuint vertexBufferID) {
        if (m_vecFilters.empty() || m_mixFilter.noIntensity() CGE_LOG_CODE(|| m_isWrapper)) {
            CGE_LOG_CODE (
                    if (m_vecFilters.empty()) {
                        CGE_LOG_ERROR("CGEMutipleEffectFilter::render2Texture did nothing!\n");
                    }

                    if (m_isWrapper) {
                        CGE_LOG_ERROR(
                                "Invalid usage!! A wrapper should not be directly rendered!\n");
                        assert(0);
                    }
            )

            handler->swapBufferFBO();

            return;
        }

        const auto &sz = handler->getOutputFBOSize();

        bool needMix = m_mixFilter.needToMix();

        if (needMix) {
            if (m_texCache == 0 || sz != m_currentSize) {
                m_currentSize = sz;
                glDeleteTextures(1, &m_texCache);
                m_texCache = cgeGenTextureWithBuffer(nullptr, m_currentSize.width,
                                                     m_currentSize.height, GL_RGBA,
                                                     GL_UNSIGNED_BYTE, 4, 0, GL_NEAREST,
                                                     GL_CLAMP_TO_EDGE);
            }

            handler->copyLastResultTexture(m_texCache);
        }

        std::vector<CGEImageFilterInterface *>::iterator iter = m_vecFilters.begin();
        for (;;) {
            (*iter)->render2Texture(handler, handler->getBufferTextureID(), vertexBufferID);

            if (++iter != m_vecFilters.end()) {
                handler->swapBufferFBO();
            } else {
                break;
            }
        }

        if (needMix) {
            handler->swapBufferFBO();
            m_mixFilter.render2Texture(handler, m_texCache, vertexBufferID);
        }
    }

    std::vector<CGEImageFilterInterface *> CGEMutipleEffectFilter::getFilters(bool bMove) {
        if (!bMove) return m_vecFilters;

        decltype(m_vecFilters) filter = m_vecFilters;
        m_vecFilters.clear();

        return filter;
    }
}
