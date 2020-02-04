/*
* cgeMultipleEffects.cpp
*
*  Created on: 2013-12-13
*      Author: Wang Yang
*        Mail: admin@wysaid.org
* 	Edited by: Niek Akerboom
*/

#include "cgeDataParsingEngine.h"
#include "cgeFilterBasic.h"

#include <cstring>
#include <cctype>
#include <sstream>
#include <math.h>

#define LOG_ERROR_PARAM(arg) CGE_LOG_ERROR("Invalid Parameters: %s\n", arg);

namespace CGE {
    extern bool g_isFastFilterImpossible;

#define ADJUSTHELP_COMMON_FUNC(str, procName, setFunc) \
    do{\
        float intensity;\
        if(sscanf(str, "%f", &intensity) != 1)\
        {\
            LOG_ERROR_PARAM(str);\
            return nullptr;\
        }\
        procName* bp = new procName();\
        if(!bp->init())\
        {\
            delete bp;\
        }\
        else \
        {\
            proc = bp;\
            bp->setFunc(intensity);\
        }\
    }while(0)

#define ADJUSTHELP_MATRIX_FUNC(str, procName, setFunc) \
    do{\
        float m[16];\
        if(sscanf(str, "%f%*c%f%*c%f%*c%f%*c%f%*c%f%*c%f%*c%f%*c%f%*c%f%*c%f%*c%f%*c%f%*c%f%*c%f%*c%f", \
            m+0, m+1, m+2, m+3, m+4, m+5, m+6, m+7, m+8, m+9, m+10, m+11, m+12, m+13, m+14, m+15) != 16)\
        {\
            LOG_ERROR_PARAM(str);\
            return nullptr;\
        }\
        procName* bp = new procName();\
        if(!bp->init())\
        {\
            delete bp;\
        }\
        else \
        {\
            proc = bp;\
            bp->setFunc(m);\
        }\
    }while(0)

    CGEImageFilterInterface *CGEDataParsingEngine::adjustParser(CGEImageHandlerAndroid *handler, const char *pstr, CGEMutipleEffectFilter *fatherFilter) {
        while (*pstr != '\0' && (*pstr == ' ' || *pstr == '\t')) ++pstr;
        CGEImageFilterInterface *proc = nullptr;

        char buffer[128], *pBuffer = buffer;

        while (*pstr != '\0' && !isspace(*pstr) && (pBuffer - buffer) < sizeof(buffer)) {
            *pBuffer++ = *pstr++;
        }

        *pBuffer = '\0';

        if (strcmp(buffer, "lut") == 0) {
            char lutName[128];

            if (sscanf(pstr, "%127s", lutName) != 1) {
                LOG_ERROR_PARAM(pstr);
                return nullptr;
            }

            CGELookupFilter *filter = createLookupFilter();
            GLuint tex = fatherFilter->loadResources(lutName);
            if (filter != nullptr && tex != 0) {

                if (strcmp(lutName, "original.png") == 0) {
                    filter->doNotUse(true);
                    glDeleteTextures(1, &tex);
                } else {
                    filter->doNotUse(false);
                    filter->setLookupTexture(tex);
                }

                proc = filter;
            } else {
                delete filter;

                glDeleteTextures(1, &tex);
                CGE_LOG_ERROR("CGEDataParsingEngine::adjustParser Create Lookup filter Failed\n");
            }
        } else if (strcmp(buffer, "contrast") == 0) {
            ADJUSTHELP_COMMON_FUNC(pstr, CGEContrastFilter, setContrast);
        } else if (strcmp(buffer, "saturation") == 0) {
            ADJUSTHELP_COMMON_FUNC(pstr, CGESaturationFilter, setSaturation);
        } else if (strcmp(buffer, "exposure") == 0) {
            ADJUSTHELP_COMMON_FUNC(pstr, CGEExposureFilter, setExposure);
        } else if (strcmp(buffer, "shadows") == 0) {
            ADJUSTHELP_COMMON_FUNC(pstr, CGEShadowsFilter, setShadows);
        } else if (strcmp(buffer, "highlights") == 0) {
            ADJUSTHELP_COMMON_FUNC(pstr, CGEHighlightsFilter, setHighlights);
        } else if (strcmp(buffer, "warmth") == 0) {
            ADJUSTHELP_COMMON_FUNC(pstr, CGEWarmthFilter, setWarmth);
        } else if (strcmp(buffer, "vignette") == 0) {
            CGEVignetteFilter *filter = createVignetteFilter();
            if (filter != nullptr) {
                float strength, centerX, centerY, widthMultiplier, heightMultiplier;
                int amountOfArguments = sscanf(pstr, "%f%*c%f%*c%f%*c%f%*c%f", &strength, &centerX, &centerY, &widthMultiplier, &heightMultiplier);

                if (amountOfArguments == 1) {
                    filter->setVignetteStrength(strength);
                } else if (amountOfArguments == 4) {
                    filter->setCenter(centerX, centerY);
                    filter->setSizeMultiplier(widthMultiplier, heightMultiplier);
                } else if (amountOfArguments == 5) {
                    filter->setVignetteStrength(strength);
                    filter->setCenter(centerX, centerY);
                    filter->setSizeMultiplier(widthMultiplier, heightMultiplier);
                } else {
                    CGE_LOG_ERROR("adjusting vignette failed: %s", pstr);
                    return nullptr;
                }

                proc = filter;
            } else {
                delete filter;
                CGE_LOG_ERROR("CGEDataParsingEngine::adjustParser Create Vignette filter Failed\n");
                return nullptr;
            }
        } else if (strcmp(buffer, "sharpen") == 0) {
            CGESharpenFilter *filter = createSharpenFilter();
            if (filter != nullptr) {
                float sharpness;
                sscanf(pstr, "%f", &sharpness);

                filter->setSharpness(sharpness);

                // there's only a handler when getting the final bitmap
                if(handler != nullptr) {
                    filter->setRenderSize(handler->getSourceWidth(), handler->getSourceHeight());
                }

                proc = filter;
            } else {
                delete filter;
                CGE_LOG_ERROR("CGEDataParsingEngine::adjustParser Create Sharpen filter Failed\n");
                return nullptr;
            }
        } else if (strcmp(buffer, "focus") == 0) {
            CGEFocusFilter *filter = createFocusFilter();
            if (filter != nullptr) {
                int textureId, textureWidth, textureHeight, focusType;
                float xPos, yPos, radius;

                int amountOfArguments = sscanf(pstr,
                    "%d%*c%d%*c%d%*c%d%*c%f%*c%f%*c%f",
                    &textureId, &textureWidth, &textureHeight, &focusType, &xPos, &yPos, &radius);

                if (amountOfArguments == 7) {
                    filter->setBlurTexture(textureId);
                    filter->setAspectRatio((float)textureHeight / (float)textureWidth);
                    filter->setFocusType(focusType);
                    filter->setFocusPosition(xPos, yPos);
                    filter->setFocusRadius(radius);
                } else {
                    filter->setFocusType(0);
                }

                proc = filter;
            } else {
                delete filter;
                CGE_LOG_ERROR("CGEDataParsingEngine::adjustParser Create Focus filter Failed\n");
                return nullptr;
            }
        } else if (strcmp(buffer, "transformation") == 0) {
            CGETransformationFilter *filter = createTransformationFilter();
            if (filter != nullptr) {
                float m[16];
                float xPos, yPos, cropWidth, cropHeight, sourceWidth, sourceHeight;

                int amountOfArguments = sscanf(pstr,
                    "%f%*c%f%*c%f%*c%f%*c%f%*c%f%*c%f%*c%f%*c%f%*c%f%*c%f%*c%f%*c%f%*c%f%*c%f%*c%f%*c%f%*c%f%*c%f%*c%f%*c%f%*c%f",
                    m + 0, m + 1, m + 2, m + 3, m + 4, m + 5, m + 6, m + 7,
                    m + 8, m + 9, m + 10, m + 11, m + 12, m + 13, m + 14, m + 15,
                    &xPos, &yPos, &cropWidth, &cropHeight, &sourceWidth, &sourceHeight);

                // there's only a handler when getting the final bitmap
                if(handler != nullptr) {
                    xPos = round(xPos * handler->getOutputScale());
                    yPos = round(yPos * handler->getOutputScale());
                    cropWidth = round(cropWidth * handler->getOutputScale());
                    cropHeight = round(cropHeight * handler->getOutputScale());

                    sourceWidth = handler->getSourceWidth();
                    sourceHeight = handler->getSourceHeight();
                }

                if (amountOfArguments == 22) {
                    filter->setTransformationMatrix(m);
                    filter->setCrop(xPos, yPos, cropWidth, cropHeight);
                    filter->setSourceSize(sourceWidth, sourceHeight);
                } else if (amountOfArguments == 20) {
                    filter->setTransformationMatrix(m);
                    filter->setCrop(xPos, yPos, cropWidth, cropHeight);
                    filter->setSourceSize(sourceWidth, sourceHeight);
                } else if (amountOfArguments == 16) {
                    filter->setTransformationMatrix(m);
                } else {
                    amountOfArguments = sscanf(pstr, "%f%*c%f%*c%f%*c%f%*c%f%*c%f", &xPos, &yPos, &cropWidth, &cropHeight, &sourceWidth, &sourceHeight);

                    if(handler != nullptr) {
                        xPos = round(xPos * handler->getOutputScale());
                        yPos = round(yPos * handler->getOutputScale());
                        cropWidth = round(cropWidth * handler->getOutputScale());
                        cropHeight = round(cropHeight * handler->getOutputScale());

                        sourceWidth = handler->getSourceWidth();
                        sourceHeight = handler->getSourceHeight();
                    }

                    if (amountOfArguments == 6) {
                        filter->setCrop(xPos, yPos, cropWidth, cropHeight);
                        filter->setSourceSize(sourceWidth, sourceHeight);
                    } else if (amountOfArguments == 4) {
                        filter->setCrop(xPos, yPos, cropWidth, cropHeight);
                    } else {
                        CGE_LOG_ERROR("adjusting transformation failed: %s", pstr);
                        return nullptr;
                    }
                }

                proc = filter;
            } else {
                delete filter;
                CGE_LOG_ERROR("CGEDataParsingEngine::adjustParser Create Crop filter Failed\n");
                return nullptr;
            }
        } else if(strcmp(buffer, "lerp") == 0) {
            auto* filter = createLerpblurFilter();

            if(filter != nullptr) {
                proc = filter;
                filter->setIntensity(0.5);
            }
        } else {
            LOG_ERROR_PARAM(pstr);
            return nullptr;
        }

        if (fatherFilter != nullptr && proc != nullptr) fatherFilter->addFilter(proc);
        return proc;
    }

    int countArguments(const char* str) {
       if (str == NULL) return 0;

       bool inSpaces = true;
       int numWords = 0;

       while (*str != NULL) {
          if (std::isspace(*str)) {
             inSpaces = true;
          } else if (inSpaces) {
             numWords++;
             inSpaces = false;
          }

          ++str;
       }

       return numWords;
    }
}
 