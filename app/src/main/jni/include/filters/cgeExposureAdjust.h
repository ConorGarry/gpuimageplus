/*
 * cgeExposureAdjust.h
 *
 *  Created on: 2015-1-29
 *      Author: Wang Yang
 *	 Edited by: Niek AKerboom
 */

#ifndef _CGEEXPOSUREADJUST_H_
#define _CGEEXPOSUREADJUST_H_

#include "cgeGLFunctions.h"

namespace CGE {
    class CGEExposureFilter : public CGEImageFilterInterface {
    public:
        void setExposure(float value);

        bool init();

    protected:
        static CGEConstString paramName;
    };
}

#endif