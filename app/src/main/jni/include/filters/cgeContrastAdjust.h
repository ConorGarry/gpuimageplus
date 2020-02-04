/*
 * cgeContrastAdjust.h
 *
 *  Created on: 2013-12-26
 *      Author: Wang Yang
 *	 Edited by: Niek Akerboom
 */

#ifndef _CGECONTRAST_ADJUST_H_
#define _CGECONTRAST_ADJUST_H_

#include "cgeGLFunctions.h"

namespace CGE {
    class CGEContrastFilter : public CGEImageFilterInterface {
    public:
        void setContrast(float value);

        bool init();

    protected:
        static CGEConstString paramName;
    };
}

#endif