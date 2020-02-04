/*
* cgeLookupFilter.cpp
*
*  Created on: 2016-7-4
*      Author: Wang Yang
* Description: 全图LUT滤镜
*   Edited by: Niek Akerboom
*/

#include "cgeLookupFilter.h"

static CGEConstString s_fsh = CGE_SHADER_STRING_PRECISION_M (
        varying vec2 textureCoordinate;
        uniform sampler2D inputImageTexture;
        uniform sampler2D lookupTexture;

        uniform bool doNotUse;

        void main() {
            vec4 textureColor = texture2D(inputImageTexture, textureCoordinate);

            if(doNotUse) {
                gl_FragColor = textureColor;
            } else {
                float blueIndex = textureColor.b * 31.0;
                vec2 texcoord = textureColor.rg;

                texcoord.y *= (1.0 / 32.0 - 1.0 / 1024.0);
                texcoord.y += floor(blueIndex) / 32.0 + 0.5 / 1024.0;

                lowp vec4 outputColorLow = texture2D(lookupTexture, texcoord);
                lowp vec4 outputColorHigh = texture2D(lookupTexture, texcoord + vec2(0.0, 1.0 / 32.0));

                gl_FragColor = mix(outputColorLow, outputColorHigh, fract(blueIndex));
            }
        }
);

namespace CGE {
    CGEConstString CGELookupFilter::paramDoNotUseName = "doNotUse";

    CGELookupFilter::CGELookupFilter() : m_lookupTexture(0) {

    }

    CGELookupFilter::~CGELookupFilter() {
        glDeleteTextures(1, &m_lookupTexture);
    }

    bool CGELookupFilter::init() {
        if (initShadersFromString(g_vshDefaultWithoutTexCoord, s_fsh)) {
            m_program.bind();
            m_program.sendUniformi("lookupTexture", 1);

            return true;
        }

        return false;
    }

    void CGELookupFilter::doNotUse(bool doNotUse) {
        m_program.bind();
        m_program.sendUniformi(paramDoNotUseName, doNotUse);
    }

    void CGELookupFilter::render2Texture(CGEImageHandlerInterface *handler, GLuint srcTexture,
                                         GLuint vertexBufferID) {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, m_lookupTexture);
        CGEImageFilterInterface::render2Texture(handler, srcTexture, vertexBufferID);
    }
}