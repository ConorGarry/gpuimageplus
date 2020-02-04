/*
* cgeRotationAdjust.cpp
*
*  Created on: 2017-07-11
* 	Edited by: Niek Akerboom
*/

#include "cgeRotationAdjust.h"
#include <math.h>

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

namespace CGE {
    CGEConstString CGERotationFilter::paramTransformationName = "transformationMatrix";
    CGEConstString CGERotationFilter::paramOrthographicName = "orthographicMatrix";

    bool CGERotationFilter::init() {
        if (initShadersFromString(s_vshTransform, g_fshDefault)) {
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

    void CGERotationFilter::setTransformationMatrix(float transformationMatrix[16]) {
        m_program.bind();
        m_program.sendUniformMat4(paramTransformationName, 1, false, transformationMatrix);
    }

    void CGERotationFilter::setOrthographicMatrix(float orthographicMatrix[16]) {
        m_program.bind();
        m_program.sendUniformMat4(paramOrthographicName, 1, false, orthographicMatrix);
    }

    void CGERotationFilter::setOrthographicMatrix(float width, float height) {
        float *orthographicMatrix = new float[16]();
        orthoM(orthographicMatrix, -1.0, 1.0, -1.0 * height / width, 1.0 * height / width, -1.0f,
               1.0f);

        setOrthographicMatrix(orthographicMatrix);
    }

    void CGERotationFilter::setRotateM(float matrix[], float a, float x, float y, float z) {
        matrix[3] = 0;
        matrix[7] = 0;
        matrix[11] = 0;
        matrix[12] = 0;
        matrix[13] = 0;
        matrix[14] = 0;
        matrix[15] = 1;

        a *= (float) (M_PI / 180.0);

        float s = (float) sin(a);
        float c = (float) cos(a);

        if (1.0 == x && 0.0 == y && 0.0 == z) {
            matrix[5] = c;
            matrix[10] = c;
            matrix[6] = s;
            matrix[9] = -s;
            matrix[1] = 0;
            matrix[2] = 0;
            matrix[4] = 0;
            matrix[8] = 0;
            matrix[0] = 1;
        } else if (0.0 == x && 1.0 == y && 0.0 == z) {
            matrix[0] = c;
            matrix[10] = c;
            matrix[8] = s;
            matrix[2] = -s;
            matrix[1] = 0;
            matrix[4] = 0;
            matrix[6] = 0;
            matrix[9] = 0;
            matrix[5] = 1;
        } else if (0.0 == x && 0.0 == y && 1.0 == z) {
            matrix[0] = c;
            matrix[5] = c;
            matrix[1] = s;
            matrix[4] = -s;
            matrix[2] = 0;
            matrix[6] = 0;
            matrix[8] = 0;
            matrix[9] = 0;
            matrix[10] = 1;
        } else {
            float len = length(x, y, z);

            if (1.0 != len) {
                float recipLen = 1.0 / len;
                x *= recipLen;
                y *= recipLen;
                z *= recipLen;
            }

            float nc = 1.0 - c;
            float xy = x * y;
            float yz = y * z;
            float zx = z * x;
            float xs = x * s;
            float ys = y * s;
            float zs = z * s;

            matrix[0] = x * x * nc + c;
            matrix[4] = xy * nc - zs;
            matrix[8] = zx * nc + ys;
            matrix[1] = xy * nc + zs;
            matrix[5] = y * y * nc + c;
            matrix[9] = yz * nc - xs;
            matrix[2] = zx * nc - ys;
            matrix[6] = yz * nc + xs;
            matrix[10] = z * z * nc + c;
        }
    }

    void CGERotationFilter::orthoM(float matrix[], float left, float right, float bottom, float top,
                                   float near, float far) {
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

    void CGERotationFilter::scaleM(float matrix[], float x, float y, float z) {
        for (int i = 0; i < 4; i++) {
            matrix[i] *= x;
            matrix[i + 4] *= y;
            matrix[i + 8] *= z;
        }
    }

    void CGERotationFilter::setIdentityM(float matrix[]) {
        for (int i = 0; i < 16; i++) {
            matrix[i] = 0;
        }

        for (int i = 0; i < 16; i += 5) {
            matrix[i] = 1.0;
        }
    }

    float CGERotationFilter::length(float x, float y, float z) {
        return (float) sqrt(x * x + y * y + z * z);
    }
}