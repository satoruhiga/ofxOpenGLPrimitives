OFX_OPENGL_PRIMITIVES_BEGIN_NAMESPACE

namespace detail {
	
typedef int NullData[0];
	
}

template <
	int Location_,
	typename DataType,
	GLenum GLType,
	int NumComponents,
	bool Normalize = false
>
struct Attribute_ {
	typedef DataType value_type;
	value_type value;

	enum {
		Location = Location_
	};

	vector<value_type> buffer;

	void reset() { buffer.clear(); }
	void push() { buffer.push_back(value); }

	void upload(Buffer* dst, size_t& offset)
	{
		dst->setSubData(buffer.data(), offset, size());
		offset += size();
	}

	void bind(VertexArray* vao, Buffer* vbo, size_t& offset, GLuint divisor)
	{
		VertexArrayBinding& binding = vao->getBindings(Location);

		binding.setFormat(GLType, NumComponents, Normalize);
		binding.setBuffer(vbo, offset);
		binding.setDivisor(divisor);
		binding.enable(Location);

		offset += size();
	}

	size_t size() const { return buffer.size() * sizeof(value_type); }
};

template <int Location, GLenum GLType, int NumComponents, bool Normalize>
struct Attribute_<Location, detail::NullData, GLType, NumComponents, Normalize> {
	typedef detail::NullData value_type;
	vector<int> buffer;
	void push() {}
	void reset() {}
	void upload(Buffer* dst, size_t& offset) {}
	void bind(VertexArray* vao, Buffer* vbo, size_t& offset, GLuint divisor) {}
};

namespace detail {
	
#define DEFINE_NULL_TYPE(N) \
	struct NullType ## N : public Attribute_<0, NullData, GL_FLOAT, 0, false> { \
		typedef NullData value_type; \
		value_type value; \
		enum { Location = -1 }; \
	};

DEFINE_NULL_TYPE(0)
DEFINE_NULL_TYPE(1)
DEFINE_NULL_TYPE(2)
DEFINE_NULL_TYPE(3)
DEFINE_NULL_TYPE(4)
DEFINE_NULL_TYPE(5)
DEFINE_NULL_TYPE(6)
DEFINE_NULL_TYPE(7)

}

OFX_OPENGL_PRIMITIVES_END_NAMESPACE