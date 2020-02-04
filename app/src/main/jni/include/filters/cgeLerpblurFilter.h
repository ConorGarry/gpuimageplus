/*
 * cgeLerpblurFilter.h
 *
 *  Created on: 2015-8-3
 *      Author: Wang Yang
 */

#ifndef _CGELERPBLURFILTER_H_
#define _CGELERPBLURFILTER_H_

#include "cgeGLFunctions.h"

namespace CGE {
	class CGELerpblurFilter : public CGEImageFilterInterface {
	public:
		~CGELerpblurFilter();
		bool init();

		enum { MAX_LERP_BLUR_INTENSITY = 12 };

		struct TextureCache {
			GLuint texID;
			CGESizei size;
		};

		void setBlurLevel(int value);

		void setIntensity(float value);

		void render2Texture(CGEImageHandlerInterface* handler, GLuint srcTexture, GLuint vertexBufferID);

		void setMipmapBase(float value);

	protected:
		void _genMipmaps(int width, int height);
		void _clearMipmaps();
		int _calcLevel(int len, int level);

	private:
		TextureCache m_texCache[MAX_LERP_BLUR_INTENSITY];
		CGESizei m_cacheTargetSize;
		int m_intensity;
		float m_mipmapBase;
		bool m_isBaseChanged;

		FrameBuffer m_framebuffer;
	};
}

#endif