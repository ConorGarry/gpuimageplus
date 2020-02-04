/*
 * cgeVignetteAdjust.cpp
 *
 *  Created on: 2014-1-22
 *      Author: Wang Yang
 *	 Edited by: Niek Akerboom
 */

#include "cgeVignetteAdjust.h"

static CGEConstString s_fshVignette = CGE_SHADER_STRING_PRECISION_M (
        uniform sampler2D inputImageTexture;
        varying
        highp vec2 textureCoordinate;

        uniform highp float vignetteStrength;
        uniform highp float centerX;
        uniform highp float centerY;
        uniform highp float widthMultiplier;
        uniform highp float heightMultiplier;

        void main() {
            lowp vec4 sourceImageColor = texture2D(inputImageTexture, textureCoordinate);
            lowp float d = distance(
                    vec2(textureCoordinate.x * widthMultiplier, textureCoordinate.y * heightMultiplier),
                    vec2(centerX * widthMultiplier, centerY * heightMultiplier));
            lowp float percent = smoothstep(0.75 - vignetteStrength, 0.75, d);
            gl_FragColor = vec4(sourceImageColor.rgb * mix(vec3(1.0), sourceImageColor.rgb * sourceImageColor.rgb, percent), sourceImageColor.a);
        }
);

namespace CGE {
    CGEConstString CGEVignetteFilter::paramStrengthName = "vignetteStrength";
    CGEConstString CGEVignetteFilter::paramCenterXName = "centerX";
    CGEConstString CGEVignetteFilter::paramCenterYName = "centerY";
    CGEConstString CGEVignetteFilter::paramWidthMultiplierName = "widthMultiplier";
    CGEConstString CGEVignetteFilter::paramHeightMultiplierName = "heightMultiplier";

    bool CGEVignetteFilter::init() {
        if (initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshVignette)) {
            setCenter(0.5, 0.5);
            setSizeMultiplier(1.0, 1.0);

            return true;
        }

        return false;
    }

    void CGEVignetteFilter::setCenter(float centerX, float centerY) {
        m_program.bind();
        m_program.sendUniformf(paramCenterXName, centerX);
        m_program.sendUniformf(paramCenterYName, centerY);
    }

    void CGEVignetteFilter::setSizeMultiplier(float widthMultiplier, float heightMultiplier) {
        m_program.bind();
        m_program.sendUniformf(paramWidthMultiplierName, widthMultiplier);
        m_program.sendUniformf(paramHeightMultiplierName, heightMultiplier);
    }

    void CGEVignetteFilter::setVignetteStrength(float value) {
        m_program.bind();
        m_program.sendUniformf(paramStrengthName, value);
    }
}