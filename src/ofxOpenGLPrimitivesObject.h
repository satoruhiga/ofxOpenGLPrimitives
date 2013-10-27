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

#pragma mark - Texture

class Texture : public OpenGLObject, public HasSize2D
{
public:
	OFX_OPENGL_PRIMITIVES_DEFINE_REFERENCE(Texture);
	
	Texture(GLsizei width, GLsizei height, GLint internalformat = GL_RGBA8, GLenum format = GL_RGBA, GLenum type = GL_UNSIGNED_BYTE, GLenum target = GL_TEXTURE_RECTANGLE, GLenum image_target = 0)
		:HasSize2D(width, height),
		target(target),
		image_target(image_target),
		internalformat(internalformat),
		format(format),
		type(type)
	{
		if (image_target == 0) image_target = target;
		
		glGenTextures(1, &object);
		assert(object != 0);
		
		bind();
		{
			glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			
			glTexImage2D(image_target,
						 0, /* mip level */
						 internalformat,
						 width,
						 height,
						 0, /* border */
						 format,
						 type,
						 0 /* data */);
		}
		unbind();
		
		checkError();
	}
	
	virtual ~Texture()
	{
		glDeleteTextures(1, &object);
	}
	
	void bind()
	{
		glBindTexture(target, object);
	}
	
	void unbind()
	{
		glBindTexture(target, 0);
	}
	
	void enable()
	{
		glEnable(target);
		bind();
	}
	
	void disable()
	{
		unbind();
		glDisable(target);
	}
	
	void update(const GLvoid *pixels)
	{
		glTexSubImage2D(image_target,
						0, /* GLint level */
						0, /* GLint xoffset */
						0, /* GLint yoffset */
						width,
						height,
						format,
						type,
						pixels);
		checkError();
	}
	
	//
	
	GLenum getTarget() const { return target; }
	GLenum getImageTarget() const { return image_target; }
	GLint getInternalFormat() const { return internalformat; }
	GLenum getFormat() const { return format; }
	GLenum getType() const { return type; }
	
	//
	
	void draw(int x, int y)
	{
		draw(x, y, width, height);
	}
	
	void draw(int x, int y, int w, int h)
	{
		enable();
		bind();
		
		ofPushMatrix();
		ofTranslate(x, y);
		
		if (target == GL_TEXTURE_2D)
		{
			glBegin(GL_QUADS);
			glTexCoord2f(0, 0);
			glVertex2f(0, 0);
			
			glTexCoord2f(1, 0);
			glVertex2f(w, 0);
			
			glTexCoord2f(1, 1);
			glVertex2f(w, h);
			
			glTexCoord2f(0, 1);
			glVertex2f(0, h);
			glEnd();
		}
		else if (target == GL_TEXTURE_RECTANGLE)
		{
			glBegin(GL_QUADS);
			glTexCoord2f(0, 0);
			glVertex2f(0, 0);
			
			glTexCoord2f(width, 0);
			glVertex2f(w, 0);
			
			glTexCoord2f(width, height);
			glVertex2f(w, h);
			
			glTexCoord2f(0, height);
			glVertex2f(0, h);
			glEnd();
		}
		
		ofPopMatrix();
		
		unbind();
		disable();
	}
	
protected:
	
	GLenum target;
	GLenum image_target;
	GLint internalformat;
	GLenum format;
	GLenum type;
	
	struct internal {
		struct custom_constructor {};
	};
	
	Texture(internal::custom_constructor, GLsizei width, GLsizei height, GLint internalformat = GL_RGBA8, GLenum format = GL_RGBA, GLenum type = GL_UNSIGNED_BYTE, GLenum target = GL_TEXTURE_RECTANGLE, GLenum image_target = 0)
		:HasSize2D(width, height),
		target(target),
		image_target(image_target),
		internalformat(internalformat),
		format(format),
		type(type)
	{}
};

#pragma mark - RenderBuffer

class RenderBuffer : public OpenGLObject, public HasSize2D
{
public:
	OFX_OPENGL_PRIMITIVES_DEFINE_REFERENCE(RenderBuffer);
	
	RenderBuffer(GLsizei width, GLsizei height, GLenum internalformat) : HasSize2D(width, height)
	{
		glGenRenderbuffers(1, &object);
		assert(object != 0);
		
		bind();
		glRenderbufferStorage(GL_RENDERBUFFER, internalformat, width, height);
		unbind();
		
		checkError();
	}
	
	~RenderBuffer()
	{
		glDeleteRenderbuffers(1, &object);
	}
	
	void bind()
	{
		glBindRenderbuffer(GL_RENDERBUFFER, object);
	}
	
	void unbind()
	{
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}
};

#pragma mark - FrameBuffer

class FrameBuffer : public OpenGLObject
{
public:
	OFX_OPENGL_PRIMITIVES_DEFINE_REFERENCE(FrameBuffer);
	
	FrameBuffer()
	{
		glGenFramebuffers(1, &object);
		assert(object != 0);
	}
	
	~FrameBuffer()
	{
		glDeleteFramebuffers(1, &object);
	}
	
	void bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, object);
	}
	
	void unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	
	void attach(Texture *tex, GLenum attachment = GL_COLOR_ATTACHMENT0)
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER,
							   attachment,
							   tex->getImageTarget(),
							   tex->getObject(),
							   0);
		
		checkStatus(glCheckFramebufferStatus(GL_FRAMEBUFFER));
		checkError();
	}
	
	void attach(RenderBuffer *rbo, GLenum attachment)
	{
		glFramebufferRenderbuffer(GL_FRAMEBUFFER,
								  attachment,
								  GL_RENDERBUFFER,
								  rbo->getObject());
		
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
