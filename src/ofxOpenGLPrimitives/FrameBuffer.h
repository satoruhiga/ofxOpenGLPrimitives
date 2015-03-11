#pragma once

#include "ofxOpenGLPrimitives/Texture.h"
#include "ofxOpenGLPrimitives/RenderBuffer.h"

OFX_OPENGL_PRIMITIVES_BEGIN_NAMESPACE

#pragma mark - FrameBuffer

class FrameBuffer : public OpenGLObject
{
public:
	OFX_OPENGL_PRIMITIVES_DEFINE_REFERENCE(FrameBuffer);
	
	FrameBuffer()
	{
		glGenFramebuffers(1, &handle);
		assert(handle != 0);
	}
	
	~FrameBuffer()
	{
		glDeleteFramebuffers(1, &handle);
	}
	
	void bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, handle);
	}
	
	void unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	
	void attach(Texture *tex, GLenum attachment = GL_COLOR_ATTACHMENT0)
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER,
							   attachment,
							   tex->getTarget(),
							   tex->getHandle(),
							   0);
		
		checkStatus(glCheckFramebufferStatus(GL_FRAMEBUFFER));
		checkError();
	}
	
	void attach(RenderBuffer *rbo, GLenum attachment)
	{
		glFramebufferRenderbuffer(GL_FRAMEBUFFER,
								  attachment,
								  GL_RENDERBUFFER,
								  rbo->getHandle());
		
		checkStatus(glCheckFramebufferStatus(GL_FRAMEBUFFER));
		checkError();
	}
	
	bool checkStatus(GLenum status)
	{
		if (GL_FRAMEBUFFER_COMPLETE == status) return true;
		
		string msg;
		
		switch (status)
		{
			case GL_FRAMEBUFFER_COMPLETE: msg = "GL_FRAMEBUFFER_COMPLETE"; break;
			case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: msg = "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT"; break;
			case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: msg = "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT"; break;
			case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER: msg = "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER"; break;
			case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER: msg = "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER"; break;
			case GL_FRAMEBUFFER_UNSUPPORTED: msg = "GL_FRAMEBUFFER_UNSUPPORTED"; break;
			case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS: msg = "GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS"; break;
			default: msg = "";
		}
		
		ofLogError("FrameBuffer") << msg;
		return false;
	}
};


OFX_OPENGL_PRIMITIVES_END_NAMESPACE