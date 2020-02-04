/*
 * cgeHighlightsAdjust.cpp
 *
 *  Created on: 2017-06-21
 *      Author: Niek Akerboom
 */

#include "cgeHighlightsAdjust.h"

const static char *const s_fshHighlights = CGE_SHADER_STRING_PRECISION_H (
        uniform sampler2D inputImageTexture;
        varying vec2 textureCoordinate;

        uniform float highlights;

        const mediump vec3
        luminanceWeighting = vec3(0.2126, 0.7152, 0.0722);

        void main() {
            vec4 source = texture2D(inputImageTexture, textureCoordinate);
            float luminance = dot(source.rgb, luminanceWeighting);

            float shadowDiff = clamp(0.5 - luminance, 0.0, 0.5);
            float shadowFactor = (sqrt(1.0 - 4.0 * shadowDiff * shadowDiff) - 1.0) * 0.5;
            shadowFactor = mix(-shadowDiff, shadowFactor, abs(0.0));
            float shadow = shadowDiff + shadowFactor;

            highp float highlightDiff = clamp(luminance - 0.5, 0.0, 0.5);
            highp float highlightFactor;

            if (highlights > 0.0) {
                highlightFactor =
                        sqrt(-4.0 * highlightDiff * highlightDiff + 4.0 * highlightDiff) * 0.5;
            } else {
                highlightFactor = (-sqrt(1.0 - 4.0 * highlightDiff * highlightDiff) + 1.0) * 0.5;
            }

            highlightFactor = mix(highlightDiff, highlightFactor, abs(highlights));
            highp float highlight = -highlightDiff + highlightFactor;

            vec3 result =
                    clamp(luminance + shadow + highlight, 0.0, 1.0) * (source.rgb / luminance);

            gl_FragColor = vec4(result.rgb, source.a);
        }
);

namespace CGE {
    CGEConstString CGEHighlightsFilter::paramName = "highlights";

    bool CGEHighlightsFilter::init() {
        return initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshHighlights);
    }

    void CGEHighlightsFilter::setHighlights(float value) {
        m_program.bind();
        m_program.sendUniformf(paramName, value);
    }
}