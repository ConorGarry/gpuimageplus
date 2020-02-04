/*
 * cgeSharpenAdjust.cpp
 *
 *  Created on: 2017-06-21
 *      Author: Niek Akerboom
 */

#include "cgeSharpenAdjust.h"

const static char *const s_fshSharpnessNew = CGE_SHADER_STRING_PRECISION_H (
        varying
        vec2 textureCoordinate;
        uniform
        sampler2D inputImageTexture;

        uniform float renderWidth;
        uniform float renderHeight;
        uniform float sharpness;

        void main() {
            gl_FragColor = vec4(0.0);

            gl_FragColor += -1.0 * texture2D(inputImageTexture, textureCoordinate +
                                                                vec2(-1.0 * (1.0 / renderWidth) *
                                                                     sharpness,
                                                                     0.0 * (1.0 / renderHeight) *
                                                                     sharpness));
            gl_FragColor += -1.0 * texture2D(inputImageTexture, textureCoordinate +
                                                                vec2(0.0 * (1.0 / renderWidth) *
                                                                     sharpness,
                                                                     -1.0 * (1.0 / renderHeight) *
                                                                     sharpness));
            gl_FragColor += 5.0 * texture2D(inputImageTexture, textureCoordinate +
                                                               vec2(0.0 * (1.0 / renderWidth) *
                                                                    sharpness,
                                                                    0.0 * (1.0 / renderHeight) *
                                                                    sharpness));
            gl_FragColor += -1.0 * texture2D(inputImageTexture, textureCoordinate +
                                                                vec2(0.0 * (1.0 / renderWidth) *
                                                                     sharpness,
                                                                     1.0 * (1.0 / renderHeight) *
                                                                     sharpness));
            gl_FragColor += -1.0 * texture2D(inputImageTexture, textureCoordinate +
                                                                vec2(1.0 * (1.0 / renderWidth) *
                                                                     sharpness,
                                                                     0.0 * (1.0 / renderHeight) *
                                                                     sharpness));
        }
);

namespace CGE {
    CGEConstString CGESharpenFilter::paramRenderWidthName = "renderWidth";
    CGEConstString CGESharpenFilter::paramRenderHeightName = "renderHeight";
    CGEConstString CGESharpenFilter::paramSharpnessName = "sharpness";

    bool CGESharpenFilter::init() {
        if (initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshSharpnessNew)) {
            m_program.bind();

            // this will be set to actual values later on
            m_program.sendUniformf(paramRenderWidthName, 500.0);
            m_program.sendUniformf(paramRenderHeightName, 500.0);

            return true;
        }

        return false;
    }

    void CGESharpenFilter::setRenderSize(float newRenderWidth, float newRenderHeight) {
        if (renderWidth == newRenderWidth && renderHeight == newRenderHeight) return;

        renderWidth = newRenderWidth;
        renderHeight = newRenderHeight;

        m_program.bind();
        m_program.sendUniformf(paramRenderWidthName, renderWidth);
        m_program.sendUniformf(paramRenderHeightName, renderHeight);
    }

    void CGESharpenFilter::setSharpness(float value) {
        m_program.bind();
        m_program.sendUniformf(paramSharpnessName, value);
    }
}