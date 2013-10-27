#pragma once

#include "ofxOpenGLPrimitivesUtil.h"

OFX_OPENGL_PRIMITIVES_BEGIN_NAMESPACE

class CubeMapTexture : public Texture
{
public:
	OFX_OPENGL_PRIMITIVES_DEFINE_REFERENCE(CubeMapTexture);
	
	CubeMapTexture(GLuint object, GLsizei width, GLsizei height, GLenum cubemap_target, GLint internalformat = GL_RGB8, GLenum format = GL_RGB, GLenum type = GL_UNSIGNED_BYTE)
	:Texture(internal::custom_constructor(), width, height, internalformat, format, type, GL_TEXTURE_CUBE_MAP, cubemap_target)
	{
		this->object = object;
		
		bind();
		{
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
};

class CubeMap : public Texture
{
public:
	OFX_OPENGL_PRIMITIVES_DEFINE_REFERENCE(CubeMap);
	
	CubeMap(GLsizei width, GLsizei height, GLint internalformat = GL_RGB8, GLenum format = GL_RGB, GLenum type = GL_UNSIGNED_BYTE)
	:Texture(internal::custom_constructor(), width, height, internalformat, format, type, GL_TEXTURE_CUBE_MAP)
	
	{
		glGenTextures(1, &object);
		assert(object != 0);
		
		bind();
		{
			glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			
			checkError();
			
			for (int i = 0; i < 6; i++)
			{
				textures[i] = new CubeMapTexture(object, width, height, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, internalformat, format, type);
				checkError();
			}
			
			checkError();
			
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			
			glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
			glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
			glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
			
			checkError();
		}
		unbind();
	}
	
	void enable()
	{
		Texture::enable();

		ofMatrix4x4 m;
		glGetFloatv(GL_MODELVIEW_MATRIX, m.getPtr());
		m.setTranslation(0, 0, 0);
		m = m.getInverse();

		ofSetMatrixMode(OF_MATRIX_TEXTURE);
		ofPushMatrix();
		ofLoadMatrix(m);
		ofSetMatrixMode(OF_MATRIX_MODELVIEW);
		
		glEnable(GL_TEXTURE_GEN_S);
		glEnable(GL_TEXTURE_GEN_T);
		glEnable(GL_TEXTURE_GEN_R);
	}
	
	void disable()
	{
		glDisable(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_GEN_T);
		glDisable(GL_TEXTURE_GEN_R);
		
		ofSetMatrixMode(OF_MATRIX_TEXTURE);
		ofPopMatrix();
		ofSetMatrixMode(OF_MATRIX_MODELVIEW);
		
		Texture::disable();
	}
	
	void draw(float size)
	{
		ofPushStyle();
		
		Texture::enable();
		
		ofSetColor(255);
		
		const float half_size = size / 2;
			
		glBegin(GL_QUADS);
		
		{
			glNormal3f(-1, 0, 0);
			
			glTexCoord3f(1, -1, -1);
			glVertex3f(half_size, -half_size, -half_size);

			glTexCoord3f(1, -1, 1);
			glVertex3f(half_size, -half_size, half_size);

			glTexCoord3f(1, 1, 1);
			glVertex3f(half_size, half_size, half_size);

			glTexCoord3f(1, 1, -1);
			glVertex3f(half_size, half_size, -half_size);
		}

		{
			glNormal3f(1, 0, 0);
			
			glTexCoord3f(-1, -1, -1);
			glVertex3f(-half_size, -half_size, -half_size);
			
			glTexCoord3f(-1, -1, 1);
			glVertex3f(-half_size, -half_size, half_size);
			
			glTexCoord3f(-1, 1, 1);
			glVertex3f(-half_size, half_size, half_size);
			
			glTexCoord3f(-1, 1, -1);
			glVertex3f(-half_size, half_size, -half_size);
		}

		{
			glNormal3f(0, -1, 0);
			
			glTexCoord3f(-1, 1, -1);
			glVertex3f(-half_size, half_size, -half_size);
			
			glTexCoord3f(-1, 1, 1);
			glVertex3f(-half_size, half_size, half_size);
			
			glTexCoord3f(1, 1, 1);
			glVertex3f(half_size, half_size, half_size);
			
			glTexCoord3f(1, 1, -1);
			glVertex3f(half_size, half_size, -half_size);
		}

		{
			glNormal3f(0, 1, 0);
			
			glTexCoord3f(-1, -1, -1);
			glVertex3f(-half_size, -half_size, -half_size);
			
			glTexCoord3f(-1, -1, 1);
			glVertex3f(-half_size, -half_size, half_size);
			
			glTexCoord3f(1, -1, 1);
			glVertex3f(half_size, -half_size, half_size);
			
			glTexCoord3f(1, -1, -1);
			glVertex3f(half_size, -half_size, -half_size);
		}

		{
			glNormal3f(0, 0, -1);
			
			glTexCoord3f(-1, -1, 1);
			glVertex3f(-half_size, -half_size, half_size);
			
			glTexCoord3f(-1, 1, 1);
			glVertex3f(-half_size, half_size, half_size);
			
			glTexCoord3f(1, 1, 1);
			glVertex3f(half_size, half_size, half_size);
			
			glTexCoord3f(1, -1, 1);
			glVertex3f(half_size, -half_size, half_size);
		}

		{
			glNormal3f(0, 0, -1);
			
			glTexCoord3f(-1, -1, -1);
			glVertex3f(-half_size, -half_size, -half_size);
			
			glTexCoord3f(-1, 1, -1);
			glVertex3f(-half_size, half_size, -half_size);
			
			glTexCoord3f(1, 1, -1);
			glVertex3f(half_size, half_size, -half_size);
			
			glTexCoord3f(1, -1, -1);
			glVertex3f(half_size, -half_size, -half_size);
		}

		glEnd();

		Texture::disable();
		
		ofPopStyle();
	}
	
	size_t size() const { return 6; }
	CubeMapTexture::Ref& getCubeMapTexture(int index) { return textures[index]; }
	
protected:
	
	CubeMapTexture::Ref textures[6];
};

OFX_OPENGL_PRIMITIVES_END_NAMESPACE
