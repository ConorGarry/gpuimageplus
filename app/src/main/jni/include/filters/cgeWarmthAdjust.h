/*
 * cgeWarmthAdjust.h
 *
 *  Created on: 2017-6-13
 *      Author: Niek Akerboom
 */

#ifndef _CGEWARMTHADJUST_H_
#define _CGEWARMTHADJUST_H_

#include "cgeGLFunctions.h"

namespace CGE {
    class CGEWarmthFilter : public CGEImageFilterInterface {
    public:
        void setWarmth(float warmth);

        bool init();

    protected:
        static CGEConstString paramName;
    };
}

#endif