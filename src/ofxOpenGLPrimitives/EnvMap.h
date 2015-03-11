#pragma once

#include "ofxOpenGLPrimitives/CubeMap.h"

OFX_OPENGL_PRIMITIVES_BEGIN_NAMESPACE

class EnvMap : public HasSize2D, protected ofNode
{
public:
	OFX_OPENGL_PRIMITIVES_DEFINE_REFERENCE(EnvMap);
	
	EnvMap(CubeMap::Ref& cubemap) : HasSize2D(cubemap->getWidth(), cubemap->getHeight())
	{
		this->cubemap = cubemap;
		
		depth = new RenderBuffer(width, height, GL_DEPTH_COMPONENT);
		checkError();
		
		fbo = new FrameBuffer;
		fbo->bind();
		fbo->attach(cubemap->getCubeMapTexture(0).get(), GL_COLOR_ATTACHMENT0);
		fbo->attach(depth.get(), GL_DEPTH_ATTACHMENT);
		fbo->unbind();
		checkError();
		
		for (int i = 0; i < 6; i++)
		{
			cameras[i].setParent(*this);
			cameras[i].setFov(90);
			cameras[i].setAspectRatio(1);
		}
		
		{
			cameras[0].lookAt(ofVec3f(1, 0, 0), ofVec3f(0, -1, 0));
			cameras[1].lookAt(ofVec3f(-1, 0, 0), ofVec3f(0, -1, 0));
			cameras[2].lookAt(ofVec3f(0, 1, 0), ofVec3f(0, 0, 1));
			cameras[3].lookAt(ofVec3f(0, -1, 0), ofVec3f(0, 0, -1));
			cameras[4].lookAt(ofVec3f(0, 0, 1), ofVec3f(0, -1, 0));
			cameras[5].lookAt(ofVec3f(0, 0, -1), ofVec3f(0, -1, 0));
		}
		
		setNearClip(10);
		setFarClip(1000);
	}
	
	void setNearClip(float v)
	{
		for (int i = 0; i < 6; i++) cameras[i].setNearClip(v);
	}
	
	void setFarClip(float v)
	{
		for (int i = 0; i < 6; i++) cameras[i].setFarClip(v);
	}
	
	void begin(int face)
	{
		assert(face >= 0 && face < 6);
		current_face = face;
		
		ofPushStyle();
		
		fbo->bind();
		fbo->attach(cubemap->getCubeMapTexture(face).get(), GL_COLOR_ATTACHMENT0);
		
		ofClear(ofGetStyle().bgColor);
		
		ofPushView();
		ofViewport(0, 0, width, height, false);
		cameras[face].begin();
	}
	
	void end()
	{
		cameras[current_face].end();
		ofPopView();
		
		fbo->unbind();
		
		ofPopStyle();
	}
	
	void debugDraw()
	{
		ofDrawAxis(100);
		
		Frustum f;
		for (int i = 0; i < 6; i++)
		{
			f.update(cameras[i]);
			f.draw();
		}
	}
	
	size_t size() const { return 6; }
	
	ofVec3f getPosition() const { return ofNode::getPosition(); }
	void setPosition(float px, float py, float pz) { ofNode::setPosition(px, py, pz); }
	void setPosition(const ofVec3f& p) { ofNode::setPosition(p); }
	
protected:
	
	int current_face;
	
	FrameBuffer::Ref fbo;
	RenderBuffer::Ref depth;
	CubeMap::Ref cubemap;
	
	ofCamera cameras[6];
};

OFX_OPENGL_PRIMITIVES_END_NAMESPACE
