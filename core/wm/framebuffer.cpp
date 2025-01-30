#pragma once
#include "framebuffer.h"

namespace wm
{
	wm::FrameBuffer wm::createFrameBuffer(unsigned int width, unsigned int hight, GLuint colorFormat)
	{

		wm::FrameBuffer buffer;

		//buffer code
		glGenFramebuffers(1, &buffer.fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, buffer.fbo);

		glGenTextures(1, &buffer.colorBuffer[0]);

		glBindTexture(GL_TEXTURE_2D, buffer.colorBuffer[0]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, hight, 0, colorFormat, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, buffer.colorBuffer[0], 0);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			printf("frame buffer is not complete!");
			return FrameBuffer();

		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		return buffer;
	}
}

