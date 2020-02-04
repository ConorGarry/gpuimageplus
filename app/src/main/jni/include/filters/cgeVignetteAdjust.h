/*
 * cgeVignetteAdjust.h
 *
 *  Created on: 2014-1-22
 *      Author: Wang Yang
 *	 Edited by: Niek Akerboom
 */

#ifndef _CGEVIGNETTEADJUST_H_
#define _CGEVIGNETTEADJUST_H_

#include "cgeGLFunctions.h"

namespace CGE {
    class CGEVignetteFilter : public CGEImageFilterInterface {
    public:
        virtual bool init();

        void setVignetteStrength(float value);

        void setCenter(float centerX, float centerY);

        void setSizeMultiplier(float widthMultiplier, float heightMultiplier);

    protected:
        static CGEConstString paramStrengthName;
        static CGEConstString paramCenterXName;
        static CGEConstString paramCenterYName;
        static CGEConstString paramWidthMultiplierName;
        static CGEConstString paramHeightMultiplierName;
    };
}

#endif