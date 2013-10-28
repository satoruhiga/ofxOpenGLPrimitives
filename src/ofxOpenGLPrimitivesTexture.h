#pragma once

#include "ofxOpenGLPrimitivesObject.h"

OFX_OPENGL_PRIMITIVES_BEGIN_NAMESPACE

#pragma mark - Texture

class Texture : public OpenGLObject, public HasSize2D
{
public:
	OFX_OPENGL_PRIMITIVES_DEFINE_REFERENCE(Texture);
	
	Texture(GLsizei width, GLsizei height,
			TextureFormat::Enum format = TextureFormat::RGBA,
			TextureInternalFormat::Enum internalformat = TextureInternalFormat::RGBA8,
			TextureType::Enum type = TextureType::UNSIGNED_BYTE,
			TextureTarget2D::Enum target = TextureTarget2D::TEXTURE_RECTANGLE,
			TextureParameterTarget::Enum parameter_target = TextureParameterTarget::TEXTURE_RECTANGLE)
	:HasSize2D(width, height),
	target(target),
	parameter_target(parameter_target),
	internalformat(internalformat),
	format(format),
	type(type)
	{
		glGenTextures(1, &object);
		assert(object != 0);
		
		bind();
		{
			glTexParameteri(parameter_target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(parameter_target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(parameter_target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(parameter_target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			
			glTexImage2D(target,
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
		glTexSubImage2D(target,
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
	
	TextureTarget2D::Enum getTarget() const { return target; }
	TextureParameterTarget::Enum getParameterTarget() const { return parameter_target; }
	TextureInternalFormat::Enum getInternalFormat() const { return internalformat; }
	TextureFormat::Enum getFormat() const { return format; }
	TextureType::Enum getType() const { return type; }
	
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
		
		if (target == TextureTarget2D::TEXTURE_2D)
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
		else if (target == TextureTarget2D::TEXTURE_RECTANGLE)
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
	
	TextureTarget2D::Enum target;
	TextureParameterTarget::Enum parameter_target;
	TextureInternalFormat::Enum internalformat;
	TextureFormat::Enum format;
	TextureType::Enum type;
	
	struct internal {
		struct custom_constructor {};
	};
	
	Texture(internal::custom_constructor,
			GLsizei width, GLsizei height,
			TextureFormat::Enum format = TextureFormat::RGBA,
			TextureInternalFormat::Enum internalformat = TextureInternalFormat::RGBA8,
			TextureType::Enum type = TextureType::UNSIGNED_BYTE,
			TextureTarget2D::Enum target = TextureTarget2D::TEXTURE_RECTANGLE,
			TextureParameterTarget::Enum parameter_target = TextureParameterTarget::TEXTURE_RECTANGLE)
	:HasSize2D(width, height),
	target(target),
	parameter_target(parameter_target),
	internalformat(internalformat),
	format(format),
	type(type)
	{}
};


OFX_OPENGL_PRIMITIVES_END_NAMESPACE