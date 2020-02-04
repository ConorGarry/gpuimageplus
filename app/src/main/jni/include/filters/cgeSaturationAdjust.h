/*
 * cgeSaturationAdjust.h
 *
 *  Created on: 2013-12-26
 *      Author: Wang Yang
 *	 Edited by: Niek Akerboom
 */

#ifndef _CGESATURATIONADJUST_H_
#define _CGESATURATIONADJUST_H_

#include "cgeGLFunctions.h"

namespace CGE {
    class CGESaturationFilter : public CGEImageFilterInterface {
    public:
        void setSaturation(float value);

        bool init();

    protected:
        static CGEConstString paramName;
    };
}

#endif