/*
 * cgeWarmthAdjust.cpp
 *
 *  Created on: 2017-6-13
 *      Author: Niek Akerboom
 */

#include "cgeWarmthAdjust.h"

static CGEConstString s_fshWarmth = CGE_SHADER_STRING_PRECISION_H (
        varying vec2 textureCoordinate;
        uniform sampler2D inputImageTexture;

        uniform vec3 warmthColor;

        // Values from "Graphics Shaders: Theory and Practice" by Bailey and Cunningham
        const vec3 luminanceWeighting = vec3(0.2125, 0.7154, 0.0721);

        void main() {
            vec4 sourceImageColor = texture2D(inputImageTexture, textureCoordinate);
            vec3 linearColor = pow(sourceImageColor.rgb, vec3(2.2));
            float luminanceChange = dot(vec3(1.0) / warmthColor, luminanceWeighting);

            vec3 color = linearColor.rgb / warmthColor;
            color /= luminanceChange;

            vec3 gammaColor = pow(color, vec3(1.0 / 2.2));

            gl_FragColor = vec4(gammaColor, sourceImageColor.a);
        }
);

namespace CGE {
    CGEConstString CGEWarmthFilter::paramName = "warmthColor";

    bool CGEWarmthFilter::init() {
        return initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshWarmth);
    }

    void CGEWarmthFilter::setWarmth(float warmth) {
        float warmthColorR, warmthColorG, warmthColorB;

        if (warmth < 0.0) {
            warmthColorR = 1.0;
            warmthColorG = 1.0 + warmth * 0.2;
            warmthColorB = 1.0 + warmth * 0.5;
        } else {
            warmthColorR = 1.0 - warmth * 0.6;
            warmthColorG = 1.0 - warmth * 0.3;
            warmthColorB = 1.0;
        }

        m_program.bind();
        m_program.sendUniformf(paramName, warmthColorR, warmthColorG, warmthColorB);
    }
}