/*
* cgeMultipleEffects.h
*
*  Created on: 2013-12-13
*      Author: Wang Yang
* 	Edited by: Niek Akerboom
*/

#ifndef _CGEDATAPARSINGENGINE_H_
#define _CGEDATAPARSINGENGINE_H_

#include "cgeMultipleEffects.h"
#include "cgeImageHandlerAndroid.h"

namespace CGE {
    class CGEDataParsingEngine {
    public:
        static CGEImageFilterInterface *adjustParser(CGEImageHandlerAndroid *handler, const char *pstr, CGEMutipleEffectFilter *fatherFilter = nullptr);
    private:
        int countArguments();
    };
}

#endif