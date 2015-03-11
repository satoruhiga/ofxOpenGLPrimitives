#pragma once

#include "ofxOpenGLPrimitives/Program.h"

OFX_OPENGL_PRIMITIVES_BEGIN_NAMESPACE

namespace Capability {

class Camera
{
public:
	
	Camera(AbstructProgram* program)
		: program(program)
	{}
	
	void setCamera(ofCamera& cam)
	{
		cam.begin();
		cam.end();
		
		setViewMatrix(cam.getModelViewMatrix());
		setProjectionMatrix(cam.getProjectionMatrix());
	}
	
	void setViewMatrix(const ofMatrix4x4& m)
	{
		program->use();
		program->setUniform("view_matrix", m);
	}

	void setProjectionMatrix(const ofMatrix4x4& m)
	{
		program->use();
		program->setUniform("projection_matrix", m);
	}

protected:
	AbstructProgram* program;
};

class ModelTransform
{
public:
	
	ModelTransform(AbstructProgram* program)
		: program(program)
	{}
	
	void setModelMatrix(const ofMatrix4x4& m)
	{
		matrix = m;
		
		program->use();
		program->setUniform("model_matrix", m);
	}
	
	void pushMatrix()
	{
		matrix_stack.push(matrix);
	}
	
	void popMatrix()
	{
		matrix_stack.pop();
		matrix = matrix_stack.top();
	}
	
	void draw(ofNode& node)
	{
		pushMatrix();
		setModelMatrix(node.getGlobalTransformMatrix());
		node.customDraw();
		popMatrix();
	}
	
protected:
	AbstructProgram* program;
	
	ofMatrix4x4 matrix;
	stack<ofMatrix4x4> matrix_stack;
};

} // Capability

OFX_OPENGL_PRIMITIVES_END_NAMESPACE