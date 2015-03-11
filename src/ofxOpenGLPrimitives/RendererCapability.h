#pragma once

#include "ofxOpenGLPrimitives/Program.h"

OFX_OPENGL_PRIMITIVES_BEGIN_NAMESPACE

namespace detail {
	
struct AbstructRendererCapability
{
	AbstructRendererCapability(AbstructProgram* program) {}
	void preDraw() {}
	void postDraw() {}
};

#define DEFINE_NULL_CAPABILITY(N) \
struct NullCapability ## N : public AbstructRendererCapability { \
NullCapability ## N(AbstructProgram* program) : AbstructRendererCapability(program) {} \
};

DEFINE_NULL_CAPABILITY(0);
DEFINE_NULL_CAPABILITY(1);
DEFINE_NULL_CAPABILITY(2);
DEFINE_NULL_CAPABILITY(3);
DEFINE_NULL_CAPABILITY(4);
DEFINE_NULL_CAPABILITY(5);
DEFINE_NULL_CAPABILITY(6);
DEFINE_NULL_CAPABILITY(7);

}

namespace RendererCapability {

class Camera : public detail::AbstructRendererCapability
{
public:
	
	Camera(AbstructProgram* program)
		: AbstructRendererCapability(program)
		, program(program)
		, camera_ptr(NULL)
	{}
	
	void setCamera(ofCamera& cam)
	{
		cam.begin();
		cam.end();
		
		camera_ptr = &cam;
	}
	
	void unsetCamera()
	{
		camera_ptr = NULL;
	}
	
	void setViewMatrix(const ofMatrix4x4& m)
	{
		view_matrix = m;
	}

	void setProjectionMatrix(const ofMatrix4x4& m)
	{
		projection_matrix = m;
	}

	void preDraw()
	{
		if (camera_ptr)
		{
			view_matrix = camera_ptr->getModelViewMatrix();
			projection_matrix = camera_ptr->getProjectionMatrix();
		}
		
		program->setUniform("view_matrix", view_matrix);
		program->setUniform("projection_matrix", projection_matrix);
	}

protected:
	AbstructProgram* program;
	
	ofCamera* camera_ptr;
	ofMatrix4x4 view_matrix, projection_matrix;
};

class ModelTransform : public detail::AbstructRendererCapability
{
public:
	
	ModelTransform(AbstructProgram* program)
		: AbstructRendererCapability(program)
		, program(program)
	{}
	
	void setModelMatrix(const ofMatrix4x4& m)
	{
		matrix = m;
	}

	void multMatrix(const ofMatrix4x4& m)
	{
		matrix.postMult(m);
	}

	void pushMatrix()
	{
		matrix_stack.push(matrix);
	}
	
	void popMatrix()
	{
		matrix = matrix_stack.top();
		matrix_stack.pop();
	}
	
	void draw(ofNode& node)
	{
		pushMatrix();
		setModelMatrix(node.getGlobalTransformMatrix());
		node.customDraw();
		popMatrix();
	}
	
	void preDraw()
	{
		checkError();
		program->setUniform("model_matrix", matrix);
		checkError();
	}
	
protected:
	AbstructProgram* program;
	
	ofMatrix4x4 matrix;
	stack<ofMatrix4x4> matrix_stack;
};

} // RendererCapability

OFX_OPENGL_PRIMITIVES_END_NAMESPACE