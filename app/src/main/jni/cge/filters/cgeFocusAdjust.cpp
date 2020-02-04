/*
 * cgeFocusAdjust.cpp
 *
 *  Created on: 2017-11-15
 *      Author: Niek Akerboom
 */

#include "cgeFocusAdjust.h"

static CGEConstString s_vshFocus = CGE_SHADER_STRING (
        attribute vec4 inputTextureCoordinate;
        attribute vec4 inputTextureCoordinate2;
        
        varying vec2 textureCoordinate;
        varying vec2 textureCoordinate2;

        void main() {
            gl_Position = position;
            textureCoordinate = inputTextureCoordinate.xy;
            textureCoordinate2 = inputTextureCoordinate2.xy;
        };
);

static CGEConstString s_fshFocus = CGE_SHADER_STRING_PRECISION_H (
        varying highp vec2 textureCoordinate;

        uniform sampler2D inputImageTexture;
        uniform sampler2D inputBlurTexture;

        uniform int focusType;
        uniform highp vec2 focusPosition;
        uniform highp float focusRadius;
        uniform highp float focusFallOffRate;
        uniform highp float aspectRatio;
        uniform highp float editBorderScale;

        uniform bool hideEditCircle;

        const int NO_FOCUS = 0;
        const int RADIAL = 1;
        const int LINEAR = 2;

        const vec4 FOCUS_EDIT_CIRCLE_COLOUR = vec4(1.0, 1.0, 1.0, 1.0);
        const float FOCUS_EDIT_BORDER_WIDTH = 0.003;

        void main() {
            vec2 uv = textureCoordinate;
            lowp vec4 sharpImageColor = texture2D(inputImageTexture, textureCoordinate);
            lowp vec4 blurredImageColor = texture2D(inputBlurTexture, textureCoordinate);

            lowp float blurIntensity = 0.0;
            highp float distanceFromCenter = 0.0;

            float finalEditCircleBorderWidth = FOCUS_EDIT_BORDER_WIDTH * editBorderScale;

            if(focusType == LINEAR || focusType == RADIAL) {
                highp vec2 textureCoordinateToUse = vec2(focusType == RADIAL ? textureCoordinate.x : focusPosition.x, (textureCoordinate.y * aspectRatio + 0.5 - 0.5 * aspectRatio));
                distanceFromCenter = distance(focusPosition, textureCoordinateToUse);
                blurIntensity = smoothstep(focusRadius - focusFallOffRate, focusRadius + focusFallOffRate, distanceFromCenter);
            }

            vec4 finalColor = mix(sharpImageColor, blurredImageColor, blurIntensity);
            float antiAliasingStep = 1.0 + smoothstep(focusRadius, focusRadius + finalEditCircleBorderWidth, distanceFromCenter) - smoothstep(focusRadius - finalEditCircleBorderWidth, focusRadius, distanceFromCenter);

            if (hideEditCircle || focusType == NO_FOCUS || (distanceFromCenter > (focusRadius + finalEditCircleBorderWidth)) || (distanceFromCenter < (focusRadius - finalEditCircleBorderWidth)))
                gl_FragColor = finalColor;
            else 
                gl_FragColor = mix(FOCUS_EDIT_CIRCLE_COLOUR, finalColor, antiAliasingStep);
        }
);

namespace CGE {
    CGEConstString CGEFocusFilter::paramBlurTextureName = "inputBlurTexture";
    CGEConstString CGEFocusFilter::paramFocusTypeName = "focusType";
    CGEConstString CGEFocusFilter::paramFocusRadiusName = "focusRadius";
    CGEConstString CGEFocusFilter::paramFocusFallOffRateName = "focusFallOffRate";
    CGEConstString CGEFocusFilter::paramAspectRatioName = "aspectRatio";
    CGEConstString CGEFocusFilter::paramFocusPositionName = "focusPosition";
    CGEConstString CGEFocusFilter::paramEditBorderScalePositionName = "editBorderScale";
    CGEConstString CGEFocusFilter::paramHideEditCirclePositionName = "hideEditCircle";

    CGEFocusFilter::CGEFocusFilter() : m_blurTexture(0) {
        // blank
    }

    CGEFocusFilter::~CGEFocusFilter() {
        glDeleteTextures(1, &m_blurTexture);
    }

    bool CGEFocusFilter::init() {
        if (initShadersFromString(g_vshDefaultWithoutTexCoord, s_fshFocus)) {
            m_program.bind();
            m_program.sendUniformi(paramBlurTextureName, 1);
            m_program.sendUniformi(paramFocusTypeName, 0);
            m_program.sendUniformf(paramFocusPositionName, xPosition, yPosition);
            m_program.sendUniformf(paramFocusRadiusName, radius);
            m_program.sendUniformf(paramFocusFallOffRateName, 0.2);
            m_program.sendUniformf(paramAspectRatioName, 0.5);
            m_program.sendUniformf(paramEditBorderScalePositionName, 1.0);
            m_program.sendUniformi(paramHideEditCirclePositionName, true);

            return true;
        }

        return false;
    }

    void CGEFocusFilter::render2Texture(CGEImageHandlerInterface *handler, GLuint srcTexture, GLuint vertexBufferID) {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, m_blurTexture);
        CGEImageFilterInterface::render2Texture(handler, srcTexture, vertexBufferID);
    }

    void CGEFocusFilter::setFocusType(int newType) {
        type = newType;

        m_program.bind();
        m_program.sendUniformi(paramFocusTypeName, type);
    }

    void CGEFocusFilter::setFocusRadius(float newFocusRadius) {
        radius = newFocusRadius;

        if(radius < MIN_RADIUS) radius = MIN_RADIUS;

        m_program.bind();
        m_program.sendUniformf(paramFocusRadiusName, radius);
    }

    void CGEFocusFilter::moveFocusRadius(float moveByRadius) {
        radius += moveByRadius;

        if(radius < MIN_RADIUS) radius = MIN_RADIUS;

        m_program.bind();
        m_program.sendUniformf(paramFocusRadiusName, radius);
    }

    void CGEFocusFilter::setFocusFallOffRate(float focusFallOffRate) {
        m_program.bind();
        m_program.sendUniformf(paramFocusFallOffRateName, focusFallOffRate);
    }

    void CGEFocusFilter::setAspectRatio(float aspectRatio) {
        m_aspectRatio = aspectRatio;

        m_program.bind();
        m_program.sendUniformf(paramAspectRatioName, aspectRatio);
    }

    void CGEFocusFilter::setFocusPosition(float focusPositionX, float focusPositionY) {
        xPosition = focusPositionX;
        yPosition = focusPositionY;

        m_program.bind();
        m_program.sendUniformf(paramFocusPositionName, xPosition, yPosition * m_aspectRatio + 0.5f - 0.5f * m_aspectRatio);
    }

    void CGEFocusFilter::moveFocusPosition(float moveByX, float moveByY) {
        xPosition += moveByX;
        yPosition += moveByY;

        m_program.bind();
        m_program.sendUniformf(paramFocusPositionName, xPosition, yPosition * m_aspectRatio + 0.5f - 0.5f * m_aspectRatio);
    }

    void CGEFocusFilter::setEditBorderScale(float editBorderScale) {
        m_program.bind();
        m_program.sendUniformf(paramEditBorderScalePositionName, editBorderScale);
    }

    void CGEFocusFilter::hideEditCircle(bool hideEditCircle) {
        m_program.bind();
        m_program.sendUniformi(paramHideEditCirclePositionName, hideEditCircle);
    }
}