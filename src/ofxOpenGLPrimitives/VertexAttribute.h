#pragma once

#include "ofxOpenGLPrimitives/VertexArray.h"

#include "detail/VertexArray.inc.h"

OFX_OPENGL_PRIMITIVES_BEGIN_NAMESPACE

template <
	typename T0 = detail::NullType0,
	typename T1 = detail::NullType1,
	typename T2 = detail::NullType2,
	typename T3 = detail::NullType3,
	typename T4 = detail::NullType4,
	typename T5 = detail::NullType5,
	typename T6 = detail::NullType6,
	typename T7 = detail::NullType7
>
class VertexAttribute_
	: public T0
	, public T1
	, public T2
	, public T3
	, public T4
	, public T5
	, public T6
	, public T7
{
public:
	
	VertexAttribute_()
		: num_vertices(0)
		, usage(GL_STATIC_DRAW)
		, divisor(0)
	{}
	
	void begin();
	void end();
	void push();
	
	size_t getNumVertices() const { return num_vertices; }
	
	void setDivisor(GLuint n) { divisor = n; }
	GLuint getDivisor() const { return divisor; }

protected:
	
	GLenum usage;
	
	size_t num_vertices;
	GLuint divisor;
	
	ofPtr<Buffer> vertex_buffer;
	
	static size_t getStride()
	{
		return sizeof(typename T0::value_type)
		+ sizeof(typename T1::value_type)
		+ sizeof(typename T2::value_type)
		+ sizeof(typename T3::value_type)
		+ sizeof(typename T4::value_type)
		+ sizeof(typename T5::value_type)
		+ sizeof(typename T6::value_type)
		+ sizeof(typename T7::value_type);
	}
	
	void reset()
	{
		num_vertices = 0;

		T0::reset();
		T1::reset();
		T2::reset();
		T3::reset();
		T4::reset();
		T5::reset();
		T6::reset();
		T7::reset();
	}
	
public:
	
	void bind(VertexArray* vao)
	{
		vertex_buffer->bind();
		
		size_t offset = 0;
		
		T0::bind(vao, vertex_buffer.get(), offset, divisor);
		T1::bind(vao, vertex_buffer.get(), offset, divisor);
		T2::bind(vao, vertex_buffer.get(), offset, divisor);
		T3::bind(vao, vertex_buffer.get(), offset, divisor);
		T4::bind(vao, vertex_buffer.get(), offset, divisor);
		T5::bind(vao, vertex_buffer.get(), offset, divisor);
		T6::bind(vao, vertex_buffer.get(), offset, divisor);
		T7::bind(vao, vertex_buffer.get(), offset, divisor);
	}
};

//

template <typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
inline void VertexAttribute_<T0, T1, T2, T3, T4, T5, T6, T7>::begin()
{
	reset();
}

template <typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
inline void VertexAttribute_<T0, T1, T2, T3, T4, T5, T6, T7>::end()
{
	vertex_buffer = ofPtr<Buffer>(new Buffer(GL_ARRAY_BUFFER));
	
	vertex_buffer->bind();
	vertex_buffer->allocate(getStride() * num_vertices, usage);
	
	size_t offset = 0;
	
	T0::upload(vertex_buffer.get(), offset);
	T1::upload(vertex_buffer.get(), offset);
	T2::upload(vertex_buffer.get(), offset);
	T3::upload(vertex_buffer.get(), offset);
	T4::upload(vertex_buffer.get(), offset);
	T5::upload(vertex_buffer.get(), offset);
	T6::upload(vertex_buffer.get(), offset);
	T7::upload(vertex_buffer.get(), offset);
}

template <typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
inline void VertexAttribute_<T0, T1, T2, T3, T4, T5, T6, T7>::push()
{
	num_vertices++;
	
	T0::push();
	T1::push();
	T2::push();
	T3::push();
	T4::push();
	T5::push();
	T6::push();
	T7::push();
}

OFX_OPENGL_PRIMITIVES_END_NAMESPACE