/*
 * cgeFocusAdjust.h
 *
 *  Created on: 2017-11-15
 *      Author: Niek Akerboom
 */

#ifndef _CGEFOCUSADJUST_H_
#define _CGEFOCUSADJUST_H_

#include "cgeGLFunctions.h"

namespace CGE {
    class CGEFocusFilter : public CGEImageFilterInterface {
    public:
    	CGEFocusFilter();
        ~CGEFocusFilter();
        bool init();

		float MIN_RADIUS = 0.05;

        // default focus values
        int type = 0; // 0 = no focus, 1 = radial, 2 = linear
		float radius = 0.2;
        float xPosition = 0.5;
        float yPosition = 0.5;

        inline void setBlurTexture(GLuint blurTexture) { m_blurTexture = blurTexture; };
        inline GLuint &blurTexture() { return m_blurTexture; }
        void render2Texture(CGEImageHandlerInterface *handler, GLuint srcTexture, GLuint vertexBufferID);

        void setFocusType(int newType); // 0 = no focus, 1 = radial and 2 = linear
        void setFocusRadius(float newRadius);
		void moveFocusRadius(float moveByRadius);
        void setFocusFallOffRate(float newFallOffRate);
        void setAspectRatio(float newAspectRatio);
        void setFocusPosition(float newPositionX, float newPositionY);
        void moveFocusPosition(float moveByX, float moveByY);
        void setEditBorderScale(float editBorderScale);
        void hideEditCircle(bool hideEditCircle);
    protected:
    	GLuint m_blurTexture;
        float m_aspectRatio;
    	static CGEConstString paramBlurTextureName;
        static CGEConstString paramFocusTypeName;
        static CGEConstString paramFocusRadiusName;
        static CGEConstString paramFocusFallOffRateName;
        static CGEConstString paramAspectRatioName;
        static CGEConstString paramFocusPositionName;
        static CGEConstString paramEditBorderScalePositionName;
        static CGEConstString paramHideEditCirclePositionName;
    };
}

#endif