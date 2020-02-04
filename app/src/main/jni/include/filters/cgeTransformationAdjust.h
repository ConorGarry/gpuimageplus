/*
 * cgeTransformationAdjust.h
 *
 *  Created on: 2017-7-14
 *      Author: Niek Akerboom
 */

#ifndef _CGETRANSFORMATIONADJUST_H_
#define _CGETRANSFORMATIONADJUST_H_

#include "cgeGLFunctions.h"

namespace CGE {
    class CGETransformationFilter : public CGEImageFilterInterface {
    public:
        void setSourceSize(float sourceWidth, float sourceHeight);

        void setCrop(float xPos, float yPos, float cropWidth, float cropHeight);

        void setTransformationMatrix(float transformationMatrix[16]);

        void setOrthographicMatrix(float orthographicMatrix[16]);

        void setOrthographicMatrix(float width, float height);

        bool init();

    private:
        void orthoM(float matrix[], float left, float right, float bottom, float top, float near,
                    float far);

        void setIdentityM(float matrix[]);

    protected:
        static CGEConstString paramTransformationName;
        static CGEConstString paramOrthographicName;
        static CGEConstString paramSourceWidthName;
        static CGEConstString paramSourceHeightName;
        static CGEConstString paramCropXPosName;
        static CGEConstString paramCropYPosName;
        static CGEConstString paramCropWidthName;
        static CGEConstString paramCropHeightName;
    };
}

#endif