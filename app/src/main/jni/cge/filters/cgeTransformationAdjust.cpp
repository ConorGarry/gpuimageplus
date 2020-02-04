/*
 * cgeTransformationAdjust.h
 *
 *  Created on: 2017-7-14
 *      Author: Niek Akerboom
 */

#include "cgeTransformationAdjust.h"

static CGEConstString s_vshTransform = CGE_SHADER_STRING (
        attribute vec2 vPosition;
        varying vec2 textureCoordinate;

        uniform mat4 transformationMatrix;
        uniform mat4 orthographicMatrix;

        void main() {
            gl_Position = transformationMatrix * vec4(vPosition, 0.0, 1.0) * orthographicMatrix;
            textureCoordinate = (vPosition.xy + 1.0) / 2.0;
        }
);

static CGEConstString s_fshTransform = CGE_SHADER_STRING_PRECISION_H (
        varying vec2 textureCoordinate;
        uniform sampler2D inputImageTexture;

        uniform float sourceWidth;
        uniform float sourceHeight;
        uniform float cropXPos;
        uniform float cropYPos;
        uniform float cropWidth;
        uniform float cropHeight;

        void main() {
            vec2 resolution = vec2(sourceWidth, sourceHeight);

            float widthFinal = cropWidth > 0.0 ? cropWidth : resolution.x;
            float heightFinal = cropHeight > 0.0 ? cropHeight : resolution.y;

            vec2 wantedScale = vec2(widthFinal / resolution.x, heightFinal / resolution.y);
            vec2 offset = vec2(cropXPos / resolution.x, cropYPos / resolution.y);

            gl_FragColor = texture2D(inputImageTexture, vec2(
                    textureCoordinate.x * wantedScale.x + offset.x,
                    textureCoordinate.y * wantedScale.y + offset.y)
            ) * vec4(1.0);
        }
);

namespace CGE {
    // Rotating
    CGEConstString CGETransformationFilter::paramTransformationName = "transformationMatrix";
    CGEConstString CGETransformationFilter::paramOrthographicName = "orthographicMatrix";

    // Cropping
    CGEConstString CGETransformationFilter::paramSourceWidthName = "sourceWidth";
    CGEConstString CGETransformationFilter::paramSourceHeightName = "sourceHeight";
    CGEConstString CGETransformationFilter::paramCropXPosName = "cropXPos";
    CGEConstString CGETransformationFilter::paramCropYPosName = "cropYPos";
    CGEConstString CGETransformationFilter::paramCropWidthName = "cropWidth";
    CGEConstString CGETransformationFilter::paramCropHeightName = "cropHeight";

    bool CGETransformationFilter::init() {
        if (initShadersFromString(s_vshTransform, s_fshTransform)) {
            float *orthographicMatrix = new float[16]();
            orthoM(orthographicMatrix, -1.0, 1.0, -1.0, 1.0, -1.0, 1.0);

            float *transformationMatrix = new float[16]();
            setIdentityM(transformationMatrix);

            m_program.bind();
            m_program.sendUniformMat4(paramOrthographicName, 1, false, orthographicMatrix);
            m_program.sendUniformMat4(paramTransformationName, 1, false, transformationMatrix);

            return true;
        }

        return false;
    }

    //////////////////////////
    //// COMBINED SETTERS ////
    //////////////////////////



    //////////////////////////
    ////// CROP SETTERS //////
    //////////////////////////

    void CGETransformationFilter::setSourceSize(float sourceWidth, float sourceHeight) {
        m_program.bind();
        m_program.sendUniformf(paramSourceWidthName, sourceWidth);
        m_program.sendUniformf(paramSourceHeightName, sourceHeight);
    }

    void
    CGETransformationFilter::setCrop(float xPos, float yPos, float cropWidth, float cropHeight) {
        m_program.bind();
        m_program.sendUniformf(paramCropXPosName, xPos);
        m_program.sendUniformf(paramCropYPosName, yPos);
        m_program.sendUniformf(paramCropWidthName, cropWidth);
        m_program.sendUniformf(paramCropHeightName, cropHeight);
    }

    //////////////////////////
    ///// MATRIX SETTERS /////
    //////////////////////////

    void CGETransformationFilter::setTransformationMatrix(float transformationMatrix[16]) {
        m_program.bind();
        m_program.sendUniformMat4(paramTransformationName, 1, false, transformationMatrix);
    }

    void CGETransformationFilter::setOrthographicMatrix(float orthographicMatrix[16]) {
        m_program.bind();
        m_program.sendUniformMat4(paramOrthographicName, 1, false, orthographicMatrix);
    }

    void CGETransformationFilter::setOrthographicMatrix(float width, float height) {
        float *orthographicMatrix = new float[16]();
        orthoM(orthographicMatrix, -1.0, 1.0, -1.0 * height / width, 1.0 * height / width, -1.0f,
               1.0f);

        setOrthographicMatrix(orthographicMatrix);
    }

    //////////////////////////
    ///// MATRIX HELPERS /////
    //////////////////////////

    void CGETransformationFilter::orthoM(float matrix[], float left, float right, float bottom,
                                         float top, float near, float far) {
        if (left == right) {
            CGE_LOG_ERROR("left == right");
        }
        if (bottom == top) {
            CGE_LOG_ERROR("bottom == top");
        }
        if (near == far) {
            CGE_LOG_ERROR("near == far");
        }

        float r_width = 1.0 / (right - left);
        float r_height = 1.0 / (top - bottom);
        float r_depth = 1.0 / (far - near);
        float x = 2.0 * (r_width);
        float y = 2.0 * (r_height);
        float z = -2.0 * (r_depth);
        float tx = -(right + left) * r_width;
        float ty = -(top + bottom) * r_height;
        float tz = -(far + near) * r_depth;

        matrix[0] = x;
        matrix[5] = y;
        matrix[10] = z;
        matrix[12] = tx;
        matrix[13] = ty;
        matrix[14] = tz;
        matrix[15] = 1.0;
        matrix[1] = 0.0;
        matrix[2] = 0.0;
        matrix[3] = 0.0;
        matrix[4] = 0.0;
        matrix[6] = 0.0;
        matrix[7] = 0.0;
        matrix[8] = 0.0;
        matrix[9] = 0.0;
        matrix[11] = 0.0;
    }

    void CGETransformationFilter::setIdentityM(float matrix[]) {
        for (int i = 0; i < 16; i++) {
            matrix[i] = 0;
        }

        for (int i = 0; i < 16; i += 5) {
            matrix[i] = 1.0;
        }
    }
}