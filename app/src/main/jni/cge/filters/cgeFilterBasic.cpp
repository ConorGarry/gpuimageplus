/*
* cgeFilterBasic.cpp
*
*  Created on: 2013-12-25
*      Author: Wang Yang
*        Mail: admin@wysaid.org
* 	Edited by: Niek Akerboom
*/

#include "cgeFilterBasic.h"

#define COMMON_FUNC(type) \
type* proc = new type();\
if(!proc->init())\
{\
    delete proc;\
    proc = NULL;\
}\
return proc;\

namespace CGE {
    CGEContrastFilter *createContrastFilter() {
        COMMON_FUNC(CGEContrastFilter);
    }

    CGEExposureFilter *createExposureFilter() {
        COMMON_FUNC(CGEExposureFilter);
    }

    CGEHighlightsFilter *createHighlightsFilter() {
        COMMON_FUNC(CGEHighlightsFilter);
    }

    CGELookupFilter *createLookupFilter() {
        COMMON_FUNC(CGELookupFilter);
    }

    CGESaturationFilter *createSaturationFilter() {
        COMMON_FUNC(CGESaturationFilter);
    }

    CGEShadowsFilter *createShadowsFilter() {
        COMMON_FUNC(CGEShadowsFilter);
    }

    CGESharpenFilter *createSharpenFilter() {
        COMMON_FUNC(CGESharpenFilter);
    }

    CGEVignetteFilter *createVignetteFilter() {
        COMMON_FUNC(CGEVignetteFilter);
    }

    CGEWarmthFilter *createWarmthFilter() {
        COMMON_FUNC(CGEWarmthFilter);
    }

    CGERotationFilter *createRotationFilter() {
        COMMON_FUNC(CGERotationFilter);
    }

    CGETransformationFilter *createTransformationFilter() {
        COMMON_FUNC(CGETransformationFilter);
    }

    CGELerpblurFilter *createLerpblurFilter() {
        COMMON_FUNC(CGELerpblurFilter);
    }

    CGEFocusFilter *createFocusFilter() {
        COMMON_FUNC(CGEFocusFilter);
    }
}
