/* cgeRotationAdjust.h
 *
 *  Created on: 2017-07-11
 *	 Edited by: Niek Akerboom
 */

#ifndef _CGEROTATIONADJUST_H_
#define _CGEROTATIONADJUST_H_

#include "cgeGLFunctions.h"

namespace CGE {
    class CGERotationFilter : public CGEImageFilterInterface {
    public:
        void setTransformationMatrix(float transformationMatrix[16]);

        void setOrthographicMatrix(float orthographicMatrix[16]);

        void setOrthographicMatrix(float width, float height);

        bool init();

    private:
        void setRotateM(float matrix[], float a, float x, float y, float z);

        void orthoM(float matrix[], float left, float right, float bottom, float top, float near,
                    float far);

        void scaleM(float matrix[], float x, float y, float z);

        void setIdentityM(float matrix[]);

        float length(float x, float y, float z);

    protected:
        static CGEConstString paramTransformationName;
        static CGEConstString paramOrthographicName;
    };
}

#endif