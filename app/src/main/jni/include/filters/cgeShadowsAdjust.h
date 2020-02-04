/*
 * cgeShadowsAdjust.h
 *
 *  Created on: 2017-06-21
 *      Author: Niek Akerboom
 */

#ifndef _CGESHADOWSADJUST_H_
#define _CGESHADOWSADJUST_H_

#include "cgeGLFunctions.h"

namespace CGE {
    class CGEShadowsFilter : public CGEImageFilterInterface {
    public:
        void setShadows(float value);

        bool init();

    protected:
        static CGEConstString paramName;
    };
}

#endif