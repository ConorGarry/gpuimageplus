/*
* cgeContrastAdjust.cpp
*
*  Created on: 2013-12-26
*      Author: Wang Yang
*	Edited by: Niek Akerboom
*/

#include "cgeContrastAdjust.h"

const static char *const s_fshContrast = CGE_SHADER_STRING_PRECISION_H (
        varying highp vec2 textureCoordinate;
        uniform sampler2D inputImageTexture;

        uniform lowp float contrast;

        void main() {
            lowp vec4 textureColor = texture2D(inputImageTexture, textureCoordinate);

            gl_FragColor = vec4(((textureColor.rgb - vec3(0.5)) * contrast + vec3(0.5)), textureColor.w);
        }
);

namespace CGE {
    CGEConstString CGEContrastFilter::paramName = "contrast";

    bool CGEContrastFilter::init() {
        return initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshContrast);
    }

    void CGEContrastFilter::setContrast(float value) {
        m_program.bind();
        m_program.sendUniformf(paramName, value);
    }
}