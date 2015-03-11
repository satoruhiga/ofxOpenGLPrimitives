#pragma once

#include "ofxOpenGLPrimitives/Object.h"

OFX_OPENGL_PRIMITIVES_BEGIN_NAMESPACE

#pragma mark - RenderBuffer

class RenderBuffer : public OpenGLObject, public HasSize2D
{
public:
	OFX_OPENGL_PRIMITIVES_DEFINE_REFERENCE(RenderBuffer);
	
	RenderBuffer(GLsizei width, GLsizei height, GLenum internalformat) : HasSize2D(width, height)
	{
		glGenRenderbuffers(1, &handle);
		assert(handle != 0);
		
		bind();
		glRenderbufferStorage(GL_RENDERBUFFER, internalformat, width, height);
		unbind();
		
		checkError();
	}
	
	~RenderBuffer()
	{
		glDeleteRenderbuffers(1, &handle);
	}
	
	void bind()
	{
		glBindRenderbuffer(GL_RENDERBUFFER, handle);
	}
	
	void unbind()
	{
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}
};

OFX_OPENGL_PRIMITIVES_END_NAMESPACE