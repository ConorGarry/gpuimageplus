/*
 * cgeSharpenAdjust.h
 *
 *  Created on: 2017-06-21
 *      Author: Niek Akerboom
 */

#ifndef _CGESHARPENADJUST_H_
#define _CGESHARPENADJUST_H_

#include "cgeGLFunctions.h"

namespace CGE {
    class CGESharpenFilter : public CGEImageFilterInterface {
        int renderWidth, renderHeight;

    public:
        void setSharpness(float value);

        void setRenderSize(float renderWidth, float renderHeight);

        bool init();

    protected:
        static CGEConstString paramRenderWidthName;
        static CGEConstString paramRenderHeightName;
        static CGEConstString paramSharpnessName;
    };
}

#endif