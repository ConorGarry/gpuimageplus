/*
 * cgeFilterBasic.h
 *
 *  Created on: 2013-12-25
 *      Author: Wang Yang
 *        Mail: admin@wysaid.org
 *	 Edited by: Niek Akerboom
 */

#ifndef _CGEBASICADJUST_H_
#define _CGEBASICADJUST_H_

#include "cgeContrastAdjust.h"
#include "cgeExposureAdjust.h"
#include "cgeHighlightsAdjust.h"
#include "cgeLookupFilter.h"
#include "cgeSaturationAdjust.h"
#include "cgeShadowsAdjust.h"
#include "cgeSharpenAdjust.h"
#include "cgeVignetteAdjust.h"
#include "cgeWarmthAdjust.h"
#include "cgeRotationAdjust.h"
#include "cgeTransformationAdjust.h"
#include "cgeLerpblurFilter.h"
#include "cgeFocusAdjust.h"

namespace CGE {
    CGEContrastFilter *createContrastFilter();

    CGEExposureFilter *createExposureFilter();

    CGEHighlightsFilter *createHighlightsFilter();

    CGELookupFilter *createLookupFilter();

    CGESaturationFilter *createSaturationFilter();

    CGEShadowsFilter *createShadowsFilter();

    CGESharpenFilter *createSharpenFilter();

    CGEVignetteFilter *createVignetteFilter();

    CGEWarmthFilter *createWarmthFilter();

    CGERotationFilter *createRotationFilter();

    CGETransformationFilter *createTransformationFilter();

    CGELerpblurFilter *createLerpblurFilter();
    
    CGEFocusFilter *createFocusFilter();
}

#endif