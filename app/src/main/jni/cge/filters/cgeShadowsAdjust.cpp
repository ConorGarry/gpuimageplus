/*
 * cgeShadowsAdjust.cpp
 *
 *  Created on: 2017-06-21
 *      Author: Niek Akerboom
 */

#include "cgeShadowsAdjust.h"

const static char *const s_fshShadows = CGE_SHADER_STRING_PRECISION_H (
        uniform
        sampler2D inputImageTexture;
        varying
        vec2 textureCoordinate;

        uniform float shadows;

        const mediump vec3
        luminanceWeighting = vec3(0.2126, 0.7152, 0.0722);

        void main() {
            vec4 source = texture2D(inputImageTexture, textureCoordinate);
            float luminance = dot(source.rgb, luminanceWeighting);

            float shadowDiff = clamp(0.5 - luminance, 0.0, 0.5);
            float shadowFactor;

            if (shadows < 0.0) {
                shadowFactor = -sqrt(-4.0 * shadowDiff * shadowDiff + 4.0 * shadowDiff) * 0.5;
            } else {
                shadowFactor = (sqrt(1.0 - 4.0 * shadowDiff * shadowDiff) - 1.0) * 0.5;
            }

            shadowFactor = mix(-shadowDiff, shadowFactor, abs(shadows));
            float shadow = shadowDiff + shadowFactor;

            float highlightDiff = clamp(luminance - 0.5, 0.0, 0.5);
            float highlightFactor = (-sqrt(1.0 - 4.0 * highlightDiff * highlightDiff) + 1.0) * 0.5;
            highlightFactor = mix(highlightDiff, highlightFactor, abs(0.0));
            float highlight = -highlightDiff + highlightFactor;

            vec3 result =
                    clamp(luminance + shadow + highlight, 0.0, 1.0) * (source.rgb / luminance);

            gl_FragColor = vec4(result.rgb, source.a);
        }
);

namespace CGE {
    CGEConstString CGEShadowsFilter::paramName = "shadows";

    bool CGEShadowsFilter::init() {
        return initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshShadows);
    }

    void CGEShadowsFilter::setShadows(float value) {
        m_program.bind();
        m_program.sendUniformf(paramName, value);
    }
}