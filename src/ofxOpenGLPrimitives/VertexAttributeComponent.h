#pragma once

#include "ofxOpenGLPrimitives/VertexAttribute.h"

OFX_OPENGL_PRIMITIVES_BEGIN_NAMESPACE

struct NullAttribute {};

template < int Location, typename DataType, GLenum GLType, int NumComponent, bool Normalize>
struct Attribute_;

///

struct Vertex : public Attribute_<0, ofVec3f, GL_FLOAT, 3>
{
	static string getAttributeName() { return "position"; }
	
	void vertex(const ofVec3f& v) { value = v; }
	void vertex(float x, float y, float z) { value.set(x, y, z); }
};

struct Normal : public Attribute_<1, ofVec3f, GL_FLOAT, 3>
{
	static string getAttributeName() { return "normal"; }
	
	void normal(const ofVec3f& v) { value = v; }
	void normal(float x, float y, float z) { value.set(x, y, z); }
};

struct Color32f : public Attribute_<2, ofFloatColor, GL_FLOAT, 4>
{
	static string getAttributeName() { return "color"; }
	
	void color(const ofFloatColor& v) { value = v; }
	void color(float r, float g, float b, float a = 1) { value.set(r, g, b, a); }
};

struct Color : public Attribute_<2, ofColor, GL_UNSIGNED_BYTE, 4, true>
{
	static string getAttributeName() { return "color"; }
	
	void color(const ofColor& v) { value = v; }
	void color(int r, int g, int b, float a = 255) { value.set(r, g, b, a); }
};

struct TexCoord0 : public Attribute_<6, ofVec2f, GL_FLOAT, 2>
{
	static string getAttributeName() { return "texcoord0"; }
	
	void texCoord0(const ofVec2f& v) { value = v; }
	void texCoord0(float x, float y) { value.set(x, y); }
};

OFX_OPENGL_PRIMITIVES_END_NAMESPACE