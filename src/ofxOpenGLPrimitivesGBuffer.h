#pragma once

#include "ofxOpenGLPrimitivesUtil.h"

OFX_OPENGL_PRIMITIVES_BEGIN_NAMESPACE

#pragma mark - GBuffer

class GBuffer
{
public:
	OFX_OPENGL_PRIMITIVES_DEFINE_REFERENCE(GBuffer);
	
	GBuffer(string name, int width, int height) : name(name), width(width), height(height) {}
	
	virtual string getUniform() { return ""; }
	virtual string getVarying() { return ""; }
	
	virtual string getVertexShader() { return ""; }
	virtual string getFragment(int render_target_index) { return ""; }
	
	string getName() const { return name; }
	Texture::Ref& getTexture() { return texture; }
	
protected:
	
	string name;
	int width, height;
	
	Texture::Ref texture;
};

#pragma mark - ColorBuffer

class ColorBuffer : public GBuffer
{
public:
	
	ColorBuffer(int width, int height) : GBuffer("color", width, height)
	{
		texture = new Texture(width, height,
							  TextureFormat::RGB,
							  TextureInternalFormat::RGB8,
							  TextureType::UNSIGNED_BYTE,
							  TextureTarget2D::TEXTURE_RECTANGLE,
							  TextureParameterTarget::TEXTURE_RECTANGLE);
	}
	
	string getVertexShader()
	{
		string s;
		s += "gl_FrontColor = gl_Color;\n";
		s += "gl_Position = ftransform();";
		return s;
	}
	
	string getFragment(int render_target_index)
	{
		return "gl_FragData[" + ofToString(render_target_index) + "] = gl_Color;";
	}
};

#pragma mark - PositionBuffer

class PositionBuffer : public GBuffer
{
public:
	
	PositionBuffer(int width, int height) : GBuffer("position", width, height)
	{
		texture = new Texture(width, height,
							  TextureFormat::RGB,
							  TextureInternalFormat::RGB16F,
							  TextureType::FLOAT,
							  TextureTarget2D::TEXTURE_RECTANGLE,
							  TextureParameterTarget::TEXTURE_RECTANGLE);
	}
	
	string getVarying()
	{
		return "varying vec3 position_vec;";
	}
	
	string getVertexShader()
	{
		return "position_vec = (gl_ModelViewMatrix * gl_Vertex).xyz;";
	}
	
	string getFragment(int render_target_index)
	{
		return "gl_FragData[" + ofToString(render_target_index) + "] = vec4(position_vec, 1);";
	}
};

#pragma mark - NormalBuffer

class NormalBuffer : public GBuffer
{
public:
	
	NormalBuffer(int width, int height) : GBuffer("normal", width, height)
	{
		texture = new Texture(width, height,
							  TextureFormat::RGB,
							  TextureInternalFormat::RGB16F,
							  TextureType::FLOAT,
							  TextureTarget2D::TEXTURE_RECTANGLE,
							  TextureParameterTarget::TEXTURE_RECTANGLE);
	}
	
	string getVarying()
	{
		return "varying vec3 normal_vec;";
	}
	
	string getVertexShader()
	{
		return "normal_vec = normalize(gl_NormalMatrix * gl_Normal);";
	}
	
	string getFragment(int render_target_index)
	{
		return "gl_FragData[" + ofToString(render_target_index) + "] = vec4(normalize(normal_vec) * 0.5 + 0.5, 1);";
	}
};

#pragma mark - GBufferFrame

class GBufferFrame
{
public:
	
	void setup(int width, int height)
	{
		this->width = width;
		this->height = height;
		
		setupBuffer(width, height);
		
		// append default color buffers without update shader
		appendColorBuffer(new ColorBuffer(width, height), false);
		appendColorBuffer(new PositionBuffer(width, height), false);
		appendColorBuffer(new NormalBuffer(width, height), false);
		
		setupShader();
	}
	
	void appendColorBuffer(GBuffer::Ref buf, bool reload_shader = true)
	{
		GLenum attachment = GL_COLOR_ATTACHMENT0 + buffers.size();
		
		fbo->bind();
		fbo->attach(buf->getTexture().get(), attachment);
		checkError();
		fbo->unbind();
		
		buffers.push_back(buf);
		target_attachments.push_back(attachment);
		
		if (reload_shader)
			setupShader();
	}

	void begin()
	{
		ofPushView();
		
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		
		ofPushStyle();
		ofPushMatrix();
		
		ofDisableBlendMode();
		ofEnableDepthTest();
		
		fbo->bind();
		glDrawBuffers(target_attachments.size(), target_attachments.data());
		
		shader.begin();
		
		checkError();
	}
	
	void end()
	{
		shader.end();
		
		fbo->unbind();
		glDrawBuffer(GL_FRONT);
		
		ofPopMatrix();
		ofPopStyle();
		glPopAttrib();
		
		ofPopView();
		
		checkError();
	}
	
	void debugDraw()
	{
		ofPushStyle();
		ofSetColor(255);
		
		int xx = 0;
		
		for (int i = 0; i < buffers.size(); i++)
		{
			GBuffer::Ref &o = buffers[i];
			Texture::Ref &tex = o->getTexture();
			
			tex->draw(xx, 0, 200, 200);
			
			xx += 200;
		}
		
		ofPopStyle();
	}
	
	Texture::Ref& getTexture(int index) { return buffers[index]->getTexture(); }
	
protected:
	
	int width, height;
	
	FrameBuffer::Ref fbo;
	RenderBuffer::Ref depth;
	vector<GBuffer::Ref> buffers;
	
	vector<GLenum> target_attachments;
	
	ofShader shader;
	
	void setupBuffer(int width, int height)
	{
		depth = new RenderBuffer(width, height, GL_DEPTH_COMPONENT);
		checkError();
		
		target_attachments.clear();
		
		fbo = new FrameBuffer;
		fbo->bind();
		fbo->attach(depth.get(), GL_DEPTH_ATTACHMENT);
		fbo->unbind();
		checkError();
	}
	
	string generateVertexShader()
	{
		stringstream ss;
		
		ss << "#version 120" << endl;
		
		for (int i = 0; i < buffers.size(); i++)
		{
			ss << "/* " << buffers[i]->getName() << " */" << endl;
			ss << buffers[i]->getVarying() << endl << endl;;
		}
		
		ss << "void main() {" << endl;
		
		for (int i = 0; i < buffers.size(); i++)
		{
			ss << "/* " << buffers[i]->getName() << " */" << endl;
			ss << buffers[i]->getVertexShader() << endl << endl;;
		}
		
		ss << "}" << endl;
		
		return ss.str();
	}
	
	string generateFragmentShader()
	{
		stringstream ss;
		
		ss << "#version 120" << endl;
		
		for (int i = 0; i < buffers.size(); i++)
		{
			ss << "/* " << buffers[i]->getName() << " */" << endl;
			ss << buffers[i]->getVarying() << endl << endl;;
		}
		
		ss << "void main() {" << endl;
		
		for (int i = 0; i < buffers.size(); i++)
		{
			ss << "/* " << buffers[i]->getName() << " */" << endl;
			ss << buffers[i]->getFragment(i) << endl << endl;;
		}
		
		ss << "}" << endl;
		
		return ss.str();
	}
	
	void setupShader()
	{
		string vs = generateVertexShader();
		string fs = generateFragmentShader();
		
		cout << vs << endl;
		cout << endl;
		cout << fs << endl;
		
		shader.setupShaderFromSource(GL_VERTEX_SHADER, vs);
		shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fs);
		
		shader.unload();
		shader.linkProgram();
		
		checkError();
	}
};

OFX_OPENGL_PRIMITIVES_END_NAMESPACE
