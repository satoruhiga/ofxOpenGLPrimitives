#pragma once

#include "ofxOpenGLPrimitives/VertexAttribute.h"

OFX_OPENGL_PRIMITIVES_BEGIN_NAMESPACE

template <
	typename T0 = detail::NullType0,
	typename T1 = detail::NullType1,
	typename T2 = detail::NullType2,
	typename T3 = detail::NullType3,
	typename T4 = detail::NullType4,
	typename T5 = detail::NullType5,
	typename T6 = detail::NullType6
>
class Geometry_
	: public VertexAttribute_<Vertex, T0, T1, T2, T3, T4, T5, T6>
{
	typedef VertexAttribute_<Vertex, T0, T1, T2, T3, T4, T5, T6> VertexAttribute;
	
public:

	void vertex(const ofVec3f& v) { Vertex::vertex(v); Geometry_::push(); }
	void vertex(float x, float y, float z) { Vertex::vertex(x, y, z); Geometry_::push(); }
	
	void begin(GLenum mode)
	{
		this->mode = mode;
		VertexAttribute::begin();
		Geometry_::reset();
	}
	
	void end() { VertexAttribute::end(); bind(); }
	
	void draw() const
	{
		vao->bind();
		glDrawElements(mode, indices.size(), GL_UNSIGNED_INT, NULL);
		vao->unbind();
	}
	
	void drawInstanced(GLsizei primcount) const
	{
		vao->bind();
		glDrawElementsInstanced(mode, indices.size(), GL_UNSIGNED_INT, NULL, primcount);
		vao->unbind();
	}
	
	void use() const { vao->bind(); }
	void release() const { vao->unbind(); }

	void restart() { indices.push_back(RESTART_INDEX); }
	
	size_t getNumIndeces() const { return indices.size(); }
	
	void setUsage(GLenum usage) { this->usage = usage; }
	
	template <typename T>
	void bindInstancedAttribute(T& v, GLuint divisor = 1)
	{
		v.setDivisor(divisor);
		
		vao->bind();
		v.bind(vao.get());
		vao->unbind();
	}
	
protected:
	
	enum {
		RESTART_INDEX = 0xFFFFFFFF
	};
	
	GLenum mode;
	std::vector<GLuint> indices;

	ofPtr<Buffer> index_buffer;
	ofPtr<VertexArray> vao;

	void bind()
	{
		vao = ofPtr<VertexArray>(new VertexArray);
		vao->bind();

		VertexAttribute::bind(vao.get());
		
		index_buffer = ofPtr<Buffer>(new Buffer(GL_ELEMENT_ARRAY_BUFFER));
		index_buffer->bind();
		index_buffer->setData(indices.data(), indices.size() * sizeof(GLuint), VertexAttribute::usage);
		
		vao->unbind();
	}
	
	void reset()
	{
		indices.clear();
		
		glEnable(GL_PRIMITIVE_RESTART);
		glPrimitiveRestartIndex(RESTART_INDEX);

		restart();
	}
	
	void push()
	{
		indices.push_back(VertexAttribute::num_vertices);
		VertexAttribute::push();
	}
};

OFX_OPENGL_PRIMITIVES_END_NAMESPACE