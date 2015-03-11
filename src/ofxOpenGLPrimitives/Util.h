#pragma once

#include "ofMain.h"

#include "ofxOpenGLPrimitives/Constants.h"

OFX_OPENGL_PRIMITIVES_BEGIN_NAMESPACE

#pragma mark - error

struct Error : public std::runtime_error
{
	Error(int err) : std::runtime_error(string((const char*)gluErrorString(err))) {}
};

bool checkError(int err);
bool checkError();

#pragma mark - auto pointer

template <typename T>
struct RefType : public ofPtr<T>
{
	RefType() : ofPtr<T>() {}
	RefType(T* ptr) : ofPtr<T>(ptr) {}
};

#define OFX_OPENGL_PRIMITIVES_DEFINE_REFERENCE(Type) typedef RefType<Type> Ref;

#pragma mark - HasSize2D

class HasSize2D
{
public:
	
	HasSize2D(GLsizei width, GLsizei height) : width(width), height(height) {}
	
	GLsizei getWidth() const { return width; }
	GLsizei getHeight() const { return height; }
	
protected:
	
	GLsizei width, height;
};

#pragma mark - Frustum

class Frustum
{
public:
	
	void update(const ofMatrix4x4& modelview_inv, const ofMatrix4x4& projection)
	{
		const ofVec3f v[] = {
			ofVec3f(-1, +1, -1),
			ofVec3f(-1, -1, -1),
			ofVec3f(+1, -1, -1),
			ofVec3f(+1, +1, -1),
			ofVec3f(-1, +1, +1),
			ofVec3f(-1, -1, +1),
			ofVec3f(+1, -1, +1),
			ofVec3f(+1, +1, +1)
		};
		
		const ofMatrix4x4 m = projection.getInverse() * modelview_inv;
		
		for (int i = 0; i < 8; i++)
		{
			points[i] = m.preMult(v[i]);
		}
		
		zero = modelview_inv.preMult(ofVec3f::zero());
	}
	
	void update(ofCamera& camera)
	{
		update(camera.getModelViewMatrix().getInverse(), camera.getProjectionMatrix());
	}
	
	void draw()
	{
		ofPushStyle();
		
		ofSetColor(255, 255, 0);
		
		glBegin(GL_LINE_LOOP);
		glVertex3fv(points[0].getPtr());
		glVertex3fv(points[1].getPtr());
		glVertex3fv(points[2].getPtr());
		glVertex3fv(points[3].getPtr());
		glEnd();
		
		glBegin(GL_LINE_LOOP);
		glVertex3fv(points[4].getPtr());
		glVertex3fv(points[5].getPtr());
		glVertex3fv(points[6].getPtr());
		glVertex3fv(points[7].getPtr());
		glEnd();
		
		glBegin(GL_LINES);
		glVertex3fv(points[0].getPtr());
		glVertex3fv(points[4].getPtr());
		
		glVertex3fv(points[1].getPtr());
		glVertex3fv(points[5].getPtr());
		
		glVertex3fv(points[2].getPtr());
		glVertex3fv(points[6].getPtr());
		
		glVertex3fv(points[3].getPtr());
		glVertex3fv(points[7].getPtr());
		glEnd();
		
		glEnable(GL_LINE_STIPPLE);
		glLineStipple(1 , 0xF0F0);
		
		glBegin(GL_LINES);
		glVertex3fv(zero.getPtr());
		glVertex3fv(points[0].getPtr());
		
		glVertex3fv(zero.getPtr());
		glVertex3fv(points[1].getPtr());
		
		glVertex3fv(zero.getPtr());
		glVertex3fv(points[2].getPtr());
		
		glVertex3fv(zero.getPtr());
		glVertex3fv(points[3].getPtr());
		glEnd();
		
		glDisable(GL_LINE_STIPPLE);
		
		ofPopStyle();
	}
	
	void getBoundingBox(ofVec3f &min, ofVec3f& max)
	{
		min.set(std::numeric_limits<float>::infinity());
		max.set(-std::numeric_limits<float>::infinity());
		
		for (int i = 0; i < 8; i++)
		{
			const ofVec3f &v = points[i];
			min.x = std::min(min.x, v.x);
			min.y = std::min(min.y, v.y);
			min.z = std::min(min.z, v.z);
			
			max.x = std::max(max.x, v.x);
			max.y = std::max(max.y, v.y);
			max.z = std::max(max.z, v.z);
		}
	}
	
	void transform(const ofMatrix4x4& m)
	{
		for (int i = 0; i < 8; i++) points[i] = m.preMult(points[i]);
		zero = m.preMult(zero);
	}
	
protected:
	
	float near, far;
	
	ofVec3f points[8];
	ofVec3f zero;
};

OFX_OPENGL_PRIMITIVES_END_NAMESPACE
