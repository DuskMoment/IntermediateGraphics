#pragma once
#include "../ew/external/glad.h"
#include <stdio.h>
namespace wm
{
	struct FrameBuffer
	{
		unsigned int fbo;
		unsigned int colorBuffer[8];
		unsigned int depthBuffer;
		unsigned int width;
		unsigned int hight;
	};
	
	FrameBuffer createFrameBuffer(unsigned int width, unsigned int hight, unsigned int colorFormat);
}