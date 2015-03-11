#include "ofxOpenGLPrimitives/Util.h"

OFX_OPENGL_PRIMITIVES_BEGIN_NAMESPACE

bool checkError(int err)
{
	if (err != GL_NO_ERROR)
	{
		ofLogError("ofxOpenGLPrimitives") << gluErrorString(err);
		// throw Error(err);
		return true;
	}
	
	return false;
}

bool checkError()
{
	return checkError(glGetError());
}

OFX_OPENGL_PRIMITIVES_END_NAMESPACE
