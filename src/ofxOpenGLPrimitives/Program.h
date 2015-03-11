#pragma once

#include "ofMain.h"

#include "ofxOpenGLPrimitives/VertexAttributeComponent.h"
#include "ofxOpenGLPrimitives/GLSLType.h"

OFX_OPENGL_PRIMITIVES_BEGIN_NAMESPACE

class Shader
{
public:
	
	static ofPtr<Shader> fromSource(GLenum shader_type, const string& code)
	{
		Shader* o = new Shader(shader_type);
		if (o->compile(code) == false) return ofPtr<Shader>();
		return ofPtr<Shader>(o);
	}

	Shader(GLenum shader_type)
		: handle(glCreateShader(shader_type))
	{}
	
	~Shader()
	{
		glDeleteShader(handle);
	}
	
	bool compile(const string& code)
	{
		const GLchar* ptr = (const GLchar*)code.c_str();
		glShaderSource(handle, 1, &ptr, NULL);
		glCompileShader(handle);
		
		GLint result;
		glGetShaderiv(handle, GL_COMPILE_STATUS, &result);
		
		if (result) return true;
		
		GLint length;
		glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &length);
		
		string err_str(length, '\0');
		GLchar* err_str_ptr = (GLchar*)err_str.c_str();
		
		glGetShaderInfoLog(handle, length, NULL, err_str_ptr);
		
		cerr << err_str;
		
		return false;
	}
	
	GLuint getHandle() const { return handle; }
	
protected:
	
	GLuint handle;
};

namespace detail {

struct AttributeData {
	GLSLType::Enum type;
	GLint size;
	string name;
	GLint location;
	
	AttributeData()
		: type(GLSLType::FLOAT)
		, size(0)
		, location(-1)
	{}
	
	AttributeData(GLSLType::Enum type, GLint size, const string& name, GLint location)
		: type(type)
		, size(size)
		, name(name)
		, location(location)
	{}
	
	AttributeData(const AttributeData& copy) { *this = copy; }
	
	AttributeData& operator=(const AttributeData& copy)
	{
		type = copy.type;
		size = copy.size;
		name = copy.name;
		location = copy.location;
		return *this;
	}
};

struct UniformData {
	GLSLType::Enum type;
	GLint size;
	string name;
	GLint location;
	
	UniformData()
		: type(GLSLType::FLOAT)
		, size(0)
		, location(-1)
	{}
	
	UniformData(GLSLType::Enum type, GLint size, const string& name, GLint location)
		: type(type)
		, size(size)
		, name(name)
		, location(location)
	{}
	
	UniformData(const UniformData& copy) { *this = copy; }
	
	UniformData& operator=(const UniformData& copy)
	{
		type = copy.type;
		size = copy.size;
		name = copy.name;
		location = copy.location;
		return *this;
	}
	
	template <typename T, int N>
	bool is_valid(GLsizei count) const
	{
		return GLSLType::type_to_enum<T, N>::value == type
		&& size == count;
	}
};
	
template <typename T>
void bind_attribute_location_helper(GLuint handle) {
	glBindAttribLocation(handle, T::Location, T::getAttributeName().c_str());
}

template <>
void bind_attribute_location_helper<NullAttribute>(GLuint handle) {
}

}

class AbstructProgram
{
public:

	AbstructProgram()
		: handle(glCreateProgram())
	{}
	
	~AbstructProgram()
	{
		glDeleteProgram(handle);
	}
	
	void attach(Shader& shader)
	{
		glAttachShader(handle, shader.getHandle());
	}
	
	template <typename T>
	void attach(const T& shader)
	{
		if (!shader) return;
		glAttachShader(handle, shader->getHandle());
	}
	
	void use() const { glUseProgram(handle); }
	void release() const { glUseProgram(NULL); }
	
	bool hasUniform(const string& name)
	{
		return uniform_map.find(name) != uniform_map.end();
	}
	
	void reset()
	{
		GLsizei count = 0;
		GLuint shaders[16];
		
		glGetAttachedShaders(handle, 16, &count, shaders);
		
		for (int i = 0; i < count; i++)
		{
			glDetachShader(handle, shaders[i]);
		}
		
		attributes.clear();
		attribute_map.clear();
		
		uniforms.clear();
		uniform_map.clear();
	}
	
	bool isLinked()
	{
		GLint result;
		glGetProgramiv(handle, GL_LINK_STATUS, &result);
		return result;
	}
	
	/// set uniforms

#define GL_UNIFORM_DEFINE_CHECK_EXISTS() \
	if (o == NULL) { \
		ofLogError() << "invalid uniform name: " << name; \
		return; \
	}
	
#define GL_UNIFORM_DEFINE_TYPE_ERROR(LONG_TYPE, NUM) \
	ofLogError() << "uniform '" << name << "' type mismatche: required " \
		<< GLSLType::to_string(o->type) << " x " << o->size \
		<< ", got " \
		<< GLSLType::to_string((GLSLType::Enum)GLSLType::type_to_enum<LONG_TYPE, NUM>::value) << " x " << count;
	
#define GL_UNIFORM_VEC_DEFINE(SHORT_TYPE, LONG_TYPE, N) \
	void setUniform ## N ## SHORT_TYPE ## v(const string& name, LONG_TYPE *data, GLsizei count) { \
		const detail::UniformData* o = getUniformData(name); \
		GL_UNIFORM_DEFINE_CHECK_EXISTS() \
		if (o->is_valid<LONG_TYPE, N>(count)) { \
			glUniform1 ## SHORT_TYPE ## v(o->location, count, data); \
		} else { GL_UNIFORM_DEFINE_TYPE_ERROR(LONG_TYPE, N) } \
	}
	
#define GL_UNIFORM_DEFINE(SHORT_TYPE, LONG_TYPE) \
	void setUniform1 ## SHORT_TYPE(const string& name, LONG_TYPE v0) { \
		const detail::UniformData* o = getUniformData(name); \
		const int count = 1; \
		GL_UNIFORM_DEFINE_CHECK_EXISTS() \
		if (o->is_valid<LONG_TYPE, 1>(count)) { \
			glUniform1 ## SHORT_TYPE(o->location, v0); \
		} else { GL_UNIFORM_DEFINE_TYPE_ERROR(LONG_TYPE, 1) } \
	} \
	void setUniform2 ## SHORT_TYPE(const string& name, LONG_TYPE v0, LONG_TYPE v1) { \
		const detail::UniformData* o = getUniformData(name); \
		const int count = 1; \
		GL_UNIFORM_DEFINE_CHECK_EXISTS() \
		if (o->is_valid<LONG_TYPE, 2>(count)) { \
			glUniform2 ## SHORT_TYPE(o->location, v0, v1); \
		} else { GL_UNIFORM_DEFINE_TYPE_ERROR(LONG_TYPE, 2) } \
	} \
	void setUniform3 ## SHORT_TYPE(const string& name, LONG_TYPE v0, LONG_TYPE v1, LONG_TYPE v2) { \
		const detail::UniformData* o = getUniformData(name); \
		const int count = 1; \
		GL_UNIFORM_DEFINE_CHECK_EXISTS() \
		if (o->is_valid<LONG_TYPE, 3>(count)) { \
			glUniform3 ## SHORT_TYPE(o->location, v0, v1, v2); \
		} else { GL_UNIFORM_DEFINE_TYPE_ERROR(LONG_TYPE, 3) } \
	} \
	void setUniform4 ## SHORT_TYPE(const string& name, LONG_TYPE v0, LONG_TYPE v1, LONG_TYPE v2, LONG_TYPE v3) { \
		const detail::UniformData* o = getUniformData(name); \
		const int count = 1; \
		GL_UNIFORM_DEFINE_CHECK_EXISTS() \
		if (o->is_valid<LONG_TYPE, 4>(count)) { \
			glUniform4 ## SHORT_TYPE(o->location, v0, v1, v2, v3); \
		} else { GL_UNIFORM_DEFINE_TYPE_ERROR(LONG_TYPE, 4) } \
	} \
	GL_UNIFORM_VEC_DEFINE(SHORT_TYPE, LONG_TYPE, 1) \
	GL_UNIFORM_VEC_DEFINE(SHORT_TYPE, LONG_TYPE, 2) \
	GL_UNIFORM_VEC_DEFINE(SHORT_TYPE, LONG_TYPE, 3) \
	GL_UNIFORM_VEC_DEFINE(SHORT_TYPE, LONG_TYPE, 4)
	
	GL_UNIFORM_DEFINE(f, float);
	GL_UNIFORM_DEFINE(i, int);
	GL_UNIFORM_DEFINE(ui, unsigned int);

#undef GL_UNIFORM_DEFINE
	
#define GL_MATRIX_UNIFORM_DEFINE(SIZE) \
	void setUniformMatrix ## SIZE ## fv(const string& name, float *data, GLsizei count, GLboolean transpose = GL_FALSE) { \
		const detail::UniformData* o = getUniformData(name); \
		GL_UNIFORM_DEFINE_CHECK_EXISTS() \
		glUniformMatrix ## SIZE ## fv(o->location, count, transpose, data); \
	}

	GL_MATRIX_UNIFORM_DEFINE(2);
	GL_MATRIX_UNIFORM_DEFINE(2x3);
	GL_MATRIX_UNIFORM_DEFINE(2x4);
	GL_MATRIX_UNIFORM_DEFINE(3);
	GL_MATRIX_UNIFORM_DEFINE(3x2);
	GL_MATRIX_UNIFORM_DEFINE(3x4);
	GL_MATRIX_UNIFORM_DEFINE(4);
	GL_MATRIX_UNIFORM_DEFINE(4x2);
	GL_MATRIX_UNIFORM_DEFINE(4x3);
	
#undef GL_MATRIX_UNIFORM_DEFINE

	void setUniform(const string& name, const ofVec2f& v) {
		const detail::UniformData* o = getUniformData(name);
		const int count = 1;
		GL_UNIFORM_DEFINE_CHECK_EXISTS()
		if (o->is_valid<float, 2>(count)) {
			glUniform2fv(o->location, count, v.getPtr());
		} else { GL_UNIFORM_DEFINE_TYPE_ERROR(float, 2) }
	}

	void setUniform(const string& name, const ofVec3f& v) {
		const detail::UniformData* o = getUniformData(name);
		const int count = 1;
		GL_UNIFORM_DEFINE_CHECK_EXISTS()
		if (o->is_valid<float, 3>(count)) {
			glUniform3fv(o->location, count, v.getPtr());
		} else { GL_UNIFORM_DEFINE_TYPE_ERROR(float, 3) }
	}

	void setUniform(const string& name, const ofVec4f& v) {
		const detail::UniformData* o = getUniformData(name);
		const int count = 1;
		GL_UNIFORM_DEFINE_CHECK_EXISTS()
		if (o->is_valid<float, 4>(count)) {
			glUniform4fv(o->location, count, v.getPtr());
		} else { GL_UNIFORM_DEFINE_TYPE_ERROR(float, 4) }
	}

	void setUniform(const string& name, const ofFloatColor& v) {
		const detail::UniformData* o = getUniformData(name);
		const int count = 1;
		GL_UNIFORM_DEFINE_CHECK_EXISTS()
		if (o->is_valid<float, 4>(count)) {
			glUniform4fv(o->location, count, &v.r);
		} else { GL_UNIFORM_DEFINE_TYPE_ERROR(float, 4) }
	}

	void setUniform(const string& name, const ofMatrix4x4& data) {
		const detail::UniformData* o = getUniformData(name);
		GL_UNIFORM_DEFINE_CHECK_EXISTS()
		glUniformMatrix4fv(o->location, 1, GL_FALSE, data.getPtr());
	}

	void dumpInfo()
	{
		cout << "active attributes:" << endl;
		for (int i = 0; i < attributes.size(); i++)
		{
			const detail::AttributeData& o = attributes[i];
			cout << "\t" << i << ": '" << o.name << "' (" << GLSLType::to_string(o.type) << " x " << o.size << ", location = " << o.location << ")" << endl;
		}
		
		cout << "active uniforms:" << endl;
		for (int i = 0; i < uniforms.size(); i++)
		{
			const detail::UniformData& o = uniforms[i];
			cout << "\t" << i << ": '" << o.name << "' (" << GLSLType::to_string(o.type) << " x " << o.size << ", location = " << o.location << ")" << endl;
		}
	}

protected:

	GLuint handle;
	
	vector<detail::AttributeData> attributes;
	map<string, detail::AttributeData*> attribute_map;
	
	vector<detail::UniformData> uniforms;
	map<string, detail::UniformData*> uniform_map;
	
	void collectProgramInfo()
	{
		{
			attributes.clear();
			attribute_map.clear();
			
			GLint num;
			GLint buf_size;
			
			glGetProgramiv(handle, GL_ACTIVE_ATTRIBUTES, &num);
			glGetProgramiv(handle, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &buf_size);
			
			for (int i = 0; i < num; i++)
			{
				GLsizei length;
				GLint size;
				GLenum type;
				string buf(buf_size, '\0');
				
				GLchar* name = (GLchar*)buf.data();
				
				glGetActiveAttrib(handle, i, buf_size, &length, &size, &type, name);
				
				GLint location = glGetAttribLocation(handle, name);
				
				detail::AttributeData data = detail::AttributeData((GLSLType::Enum)type, size, name, location);
				attributes.push_back(data);
			}
			
			for (int i = 0; i < attributes.size(); i++)
			{
				detail::AttributeData& data = attributes[i];
				attribute_map[data.name] = &data;
			}
		}
		
		{
			uniforms.clear();
			uniform_map.clear();
			
			GLint num;
			GLint buf_size;
			
			glGetProgramiv(handle, GL_ACTIVE_UNIFORMS, &num);
			glGetProgramiv(handle, GL_ACTIVE_UNIFORM_MAX_LENGTH, &buf_size);
			
			for (int i = 0; i < num; i++)
			{
				GLsizei length;
				GLint size;
				GLenum type;
				string buf(buf_size, '\0');
				
				GLchar* name = (GLchar*)buf.data();
				
				glGetActiveUniform(handle, i, buf_size, &length, &size, &type, name);
				
				GLint location = glGetUniformLocation(handle, name);
				
				detail::UniformData data = detail::UniformData((GLSLType::Enum)type, size, name, location);
				uniforms.push_back(data);
				uniform_map[name] = &uniforms.back();
			}
			
			for (int i = 0; i < uniforms.size(); i++)
			{
				detail::UniformData& data = uniforms[i];
				uniform_map[data.name] = &data;
			}
		}
	}
	
	const detail::UniformData* getUniformData(const string& name)
	{
		const map<string, detail::UniformData*>::iterator it = uniform_map.find(name);
		if (it == uniform_map.end()) return NULL;
		return it->second;
	}
};

template <
	typename T0 = NullAttribute,
	typename T1 = NullAttribute,
	typename T2 = NullAttribute,
	typename T3 = NullAttribute,
	typename T4 = NullAttribute,
	typename T5 = NullAttribute,
	typename T6 = NullAttribute,
	typename T7 = NullAttribute
>
class Program_ : public AbstructProgram
{
public:
	
	bool link()
	{
		detail::bind_attribute_location_helper<Vertex>(handle);
		detail::bind_attribute_location_helper<T0>(handle);
		detail::bind_attribute_location_helper<T1>(handle);
		detail::bind_attribute_location_helper<T2>(handle);
		detail::bind_attribute_location_helper<T3>(handle);
		detail::bind_attribute_location_helper<T4>(handle);
		detail::bind_attribute_location_helper<T5>(handle);
		detail::bind_attribute_location_helper<T6>(handle);
		detail::bind_attribute_location_helper<T7>(handle);
		
		glLinkProgram(handle);
		
		GLint result;
		glGetProgramiv(handle, GL_LINK_STATUS, &result);
		
		if (result)
		{
			collectProgramInfo();
			return true;
		}
		
		GLint length;
		glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &length);
		
		string err_str(length, '\0');
		GLchar* err_str_ptr = (GLchar*)err_str.c_str();
		
		glGetProgramInfoLog(handle, length, NULL, err_str_ptr);
		
		cerr << err_str;
		
		return false;
	}
};

OFX_OPENGL_PRIMITIVES_END_NAMESPACE