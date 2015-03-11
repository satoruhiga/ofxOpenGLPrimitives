#pragma once

#include "ofMain.h"

#include "ofxOpenGLPrimitives/Object.h"

OFX_OPENGL_PRIMITIVES_BEGIN_NAMESPACE

class VertexArray;

struct VertexArrayBinding
{
	VertexArray *vao;
	
	Buffer* buffer;
	
	GLenum type;
	GLint size;
	GLboolean normalized;
	
	GLsizei offset;
	GLsizei stride;
	
	GLuint divisor;
	
	void setFormat(GLenum type, int size, GLboolean normalized = GL_FALSE)
	{
		this->type = type;
		this->size = size;
		this->normalized = normalized;
	}
	
	void setBuffer(Buffer* buffer, GLsizei offset, GLsizei stride = 0)
	{
		this->buffer = buffer;
		this->offset = offset;
		this->stride = stride;
	}
	
	void setDivisor(GLuint divisor)
	{
		this->divisor = divisor;
	}
	
	void enable(GLuint location);
};

class VertexArray
{
public:
	
	VertexArray()
	{
		glGenVertexArrays(1, &handle);
	}
	
	~VertexArray()
	{
		glDeleteVertexArrays(1, &handle);
	}
	
	VertexArrayBinding& getBindings(int index)
	{
		if (bindings.find(index) == bindings.end())
			bindings[index].vao = this;
		
		return bindings[index];
	}
	
	void bind() const
	{
		glBindVertexArray(handle);
	}
	
	void unbind()
	{
		glBindVertexArray(NULL);
	}
	
protected:
	
	GLuint handle;
	
	map<GLuint, VertexArrayBinding> bindings;
};

///

void VertexArrayBinding::enable(GLuint location)
{
	buffer->bind();
	
	glVertexAttribPointer(location, size, type, normalized, stride, (GLvoid*)offset);
	glEnableVertexAttribArray(location);
	glVertexAttribDivisor(location, divisor);
}

OFX_OPENGL_PRIMITIVES_END_NAMESPACE