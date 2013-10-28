#pragma once

#include "ofxOpenGLPrimitivesUtil.h"

OFX_OPENGL_PRIMITIVES_BEGIN_NAMESPACE

#pragma mark - OpenGLObject

class OpenGLObject
{
public:
	
	GLuint getObject() const { return object; }
	
protected:
	
	GLuint object;
	
	OpenGLObject() : object(0) {}
	virtual ~OpenGLObject() {}
	
	virtual void bind() = 0;
	virtual void unbind() = 0;
	
private:
	
	OpenGLObject(const OpenGLObject&) {}
	OpenGLObject& operator=(const OpenGLObject&) {}
};

#pragma mark - BufferObject

class BufferObject : public OpenGLObject
{
public:
	
	BufferObject(GLenum target, GLenum usage) : target(target), usage(usage)
	{
		glGenBuffers(1, &object);
		assert(object != 0);
	}
	
	~BufferObject()
	{
		glDeleteBuffers(1, &object);
	}
	
	//
	
	void bind()
	{
		glBindBuffer(target, object);
	}
	
	void unbind()
	{
		glBindBuffer(target, 0);
	}
	
	//
	
	inline void allocate(const GLvoid *data, GLsizeiptr num_bytes)
	{
		this->num_bytes = num_bytes;
		
		glBufferData(target, num_bytes, data, usage);
		checkError();
	}
	
	template <typename T>
	inline void allocate(const vector<T>& data)
	{
		allocate(usage, data.data(), sizeof(T) * data.size());
	}
	
	inline void allocate(GLsizeiptr num_bytes)
	{
		allocate(NULL, num_bytes);
	}
	
	template <typename T>
	inline void allocate(size_t size)
	{
		allocate(sizeof(T) * size);
	}
	
	//
	
	void assign(const GLvoid *data, GLsizeiptr num_bytes, GLintptr offset = 0)
	{
		assert(this->num_bytes > 0);
		assert(this->num_bytes >= num_bytes + offset);
		
		glBufferSubData(target, offset, num_bytes, data);
		checkError();
	}
	
	template <typename T>
	void assign(const vector<T>& data)
	{
		assign(data.data(), sizeof(T) * data.size());
	}
	
	//
	
	void* map(GLenum access)
	{
		return glMapBuffer(target, access);
	}
	
	void unmap()
	{
		glUnmapBuffer(target);
	}
	
protected:
	
	GLenum target;
	GLenum usage;
	
	size_t num_bytes;
};

#pragma mark - VertexBuffer

class VertexBuffer : public BufferObject
{
public:
	OFX_OPENGL_PRIMITIVES_DEFINE_REFERENCE(VertexBuffer);
	
	VertexBuffer(GLenum target, GLenum usage) : BufferObject(target, usage) {}
	
	//
	
	inline void pushClientState(GLenum array)
	{
		glEnableClientState(array);
		getClientStateStack().push(array);
	}
	
	inline void popClientState()
	{
		glDisableClientState(getClientStateStack().top());
		getClientStateStack().pop();
	}
	
	//
	
	void setVertexPointer(GLint size = 3, GLenum type = GL_FLOAT, GLvoid *pointer = NULL)
	{
		glVertexPointer(size, type, 0, pointer);
	}
	
	void setColorPointer(GLint size = 4, GLenum type = GL_FLOAT, GLvoid *pointer = NULL)
	{
		glColorPointer(size, type, 0, pointer);
	}
	
	void setNormalPointer(GLenum type = GL_FLOAT, GLvoid *pointer = NULL)
	{
		glNormalPointer(type, 0, pointer);
	}
	
	void setIndexPointer(GLenum type = GL_UNSIGNED_INT, GLvoid *pointer = NULL)
	{
		glIndexPointer(type, 0, pointer);
	}
	
	void setTexCoordPointer(GLint size = 2, GLenum type = GL_FLOAT, GLvoid *pointer = NULL)
	{
		glTexCoordPointer(size, type, 0, pointer);
	}
	
	void setInterleaved(GLenum format = GL_C4F_N3F_V3F)
	{
		glInterleavedArrays(format, 0, NULL);
	}
	
	//
	
	void draw(GLenum mode, GLsizei num_vertices, GLenum type, const GLvoid *indices = NULL)
	{
		glDrawElements(mode, num_vertices, type, indices);
	}
	
	void draw(GLenum mode, GLsizei num_vertices)
	{
		glDrawArrays(mode, 0, num_vertices);
	}
	
private:
	
	inline static stack<GLenum>& getClientStateStack() { static stack<GLenum> s; return s; }
};

#pragma mark - PixelBuffer

class PixelBuffer : public BufferObject
{
public:
	OFX_OPENGL_PRIMITIVES_DEFINE_REFERENCE(PixelBuffer);
	
	// TODO: constructor takes width and height param
	PixelBuffer(GLenum target, GLenum usage) : BufferObject(target, usage) {}
	
	void copy(int x, int y, int width, int height, GLenum format = GL_RGBA, GLenum type = GL_UNSIGNED_BYTE)
	{
		glReadPixels(x, y, width, height, format, type, NULL);
		checkError();
	}
	
	void copy(int width, int height, GLenum format = GL_RGBA, GLenum type = GL_UNSIGNED_BYTE)
	{
		copy(0, 0, width, height, format, type);
	}
	
	void setReadBuffer(GLenum mode)
	{
		glReadBuffer(mode);
	}
	
	void setDrawBuffer(GLenum mode)
	{
		glDrawBuffer(mode);
	}
	
protected:
	
	GLsizei width;
	GLsizei height;
	GLenum format;
	GLenum type;
};

OFX_OPENGL_PRIMITIVES_END_NAMESPACE
