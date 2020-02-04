/*
 * cgeHighlightsAdjust.h
 *
 *  Created on: 2017-06-21
 *      Author: Niek Akerboom
 */

#ifndef _CGEHIGHLIGHTSADJUST_H_
#define _CGEHIGHLIGHTSADJUST_H_

#include "cgeGLFunctions.h"

namespace CGE {
    class CGEHighlightsFilter : public CGEImageFilterInterface {
    public:
        void setHighlights(float value);

        bool init();

    protected:
        static CGEConstString paramName;
    };
}

#endif