#pragma once

#include "ofMain.h"

#include "ofxOpenGLPrimitives/Constants.h"

OFX_OPENGL_PRIMITIVES_BEGIN_NAMESPACE

class ShaderLoader
{
public:
	
	ShaderLoader()
	{
		setVersion(120);
		setShaderDirectory("");
		setFileExt(".glsl");
	}
	
	void setShaderDirectory(const string& path)
	{
		shader_dir = ofToDataPath(path);
	}
	
	void setFileExt(const string& ext) { file_ext = ext; }
	void setVersion(int version) { this->version = version; }
	
	bool has(const string& key)
	{
		return repo.find(key) != repo.end();
	}
	
	const string& getShaderSource(const string& key)
	{
		return repo[key];
	}
	
	bool load(const string& name)
	{
		string path = ofFilePath::join(shader_dir, name + file_ext);
		if (ofFile::doesFileExist(path) == false) return;
		
		ofBuffer buf = ofBufferFromFile(path);
		
		string tag;
		string code;
		
		repo.clear();
		
		int line_no = 0;
		
		while (buf.isLastLine() == false)
		{
			string line = buf.getNextLine();
			
			if (line.substr(0, 2) == "--")
			{
				if (tag.empty() == false)
				{
					repo[tag] = code;
				}
				
				stringstream ss;
				ss << line;
				ss >> tag >> tag;
				
				code.clear();
				
				code += "#version " + ofToString(version) + "\n";
				code += "#line " + ofToString(line_no) + "\n";
			}
			else
			{
				code += line + "\n";
			}
			
			line_no++;
		}
		
		if (tag.empty() == false)
			repo[tag] = code;
		
		return true;
	}
	
	template <typename Program>
	bool load(const string& name, Program& program)
	{
		if (load(name) == false) return false;
		
		if (program.isLinked())
			program.reset();
		
		if (has("geometry"))
			program.attach(Shader::fromSource(GL_GEOMETRY_SHADER, getShaderSource("geometry")));
		
		if (has("vertex"))
			program.attach(Shader::fromSource(GL_VERTEX_SHADER, getShaderSource("vertex")));
		
		if (has("fragment"))
			program.attach(Shader::fromSource(GL_FRAGMENT_SHADER, getShaderSource("fragment")));
		
		program.link();
		
		return program.isLinked();
	}

	bool load(const string& name, ofShader& shader)
	{
		if (load(name) == false) return false;
		
		if (shader.isLoaded())
			shader.unload();
		
		if (has("geometry"))
			shader.setupShaderFromSource(GL_GEOMETRY_SHADER, getShaderSource("geometry"));
		
		if (has("vertex"))
			shader.setupShaderFromSource(GL_VERTEX_SHADER, getShaderSource("vertex"));
		
		if (has("fragment"))
			shader.setupShaderFromSource(GL_FRAGMENT_SHADER, getShaderSource("fragment"));
		
		shader.linkProgram();
		
		return shader.isLoaded();
	}
	
private:
	
	int version;
	string shader_dir;
	string file_ext;
	map<string, string> repo;
};

OFX_OPENGL_PRIMITIVES_END_NAMESPACE
