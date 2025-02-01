#pragma once
#include "framebuffer.h"

namespace wm
{
	FrameBuffer createFrameBuffer(unsigned int width, unsigned int hight, GLuint colorFormat, DepthType type)
	{

		wm::FrameBuffer buffer;
		buffer.width = width;
		buffer.hight = hight;

		//buffer code
		glGenFramebuffers(1, &buffer.fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, buffer.fbo);

		//create color
		glGenTextures(1, &buffer.colorBuffer[0]);

		glBindTexture(GL_TEXTURE_2D, buffer.colorBuffer[0]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, hight, 0, colorFormat, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		


		if (type == RENDER_BUFFER)
		{
			//create depth
			glGenRenderbuffers(1, &buffer.depthBuffer);

			glBindRenderbuffer(GL_RENDERBUFFER, buffer.depthBuffer);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, hight);
			glBindRenderbuffer(GL_RENDERBUFFER, 0);

			
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, buffer.depthBuffer);
		}
		else if (type == TEXTURE)
		{
			/*glGenTextures(1, &buffer.depthBuffer);

			glBindTexture(GL_TEXTURE_2D, buffer.depthBuffer);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, hight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, buffer.depthBuffer, 0);*/

			glGenTextures(1, &buffer.depthBuffer);

			glBindTexture(GL_TEXTURE_2D, buffer.depthBuffer);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, hight, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, buffer.depthBuffer, 0);

			//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, buffer.colorBuffer[0], 0);


		}
		
		//attach buffers	
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

