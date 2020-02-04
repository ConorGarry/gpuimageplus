/*
* cgeSaturationAdjust.cpp
*
*  Created on: 2013-12-26
*      Author: Wang Yang
* 	Edited by: Niek Akerboom
*/

#include "cgeSaturationAdjust.h"

static CGEConstString s_fshSaturation = CGE_SHADER_STRING_PRECISION_H (
        varying
        vec2 textureCoordinate;
        uniform
        sampler2D inputImageTexture;

        uniform float saturation;

        // Values from "Graphics Shaders: Theory and Practice" by Bailey and Cunningham
        const vec3 luminanceWeighting = vec3(0.2125, 0.7154, 0.0721);

        void main() {
            vec4 textureColor = texture2D(inputImageTexture, textureCoordinate);
            float luminance = dot(textureColor.rgb, luminanceWeighting);
            vec3 greyScaleColor = vec3(luminance);

            gl_FragColor = vec4(mix(greyScaleColor, textureColor.rgb, saturation), textureColor.w);
        }
);

namespace CGE {
    CGEConstString CGESaturationFilter::paramName = "saturation";

    bool CGESaturationFilter::init() {
        return initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshSaturation);
    }

    void CGESaturationFilter::setSaturation(float value) {
        m_program.bind();
        m_program.sendUniformf(paramName, value);
    }
}