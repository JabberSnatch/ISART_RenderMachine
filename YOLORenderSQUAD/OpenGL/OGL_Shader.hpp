#ifndef __OGL_SHADER_HPP__
#define __OGL_SHADER_HPP__

#include <glew\include\GL\glew.h>
#include <string>


class OGL_Shader
{
	enum RESOURCES
	{
		VERTEX = 0,
		FRAGMENT,
		PROGRAM,

		RESOURCES_COUNT
	};

public:
	OGL_Shader() = default;
	OGL_Shader(const OGL_Shader&) = delete;
	OGL_Shader(OGL_Shader&&) = delete;
	~OGL_Shader();

	auto	EnableShader() -> void;

	auto	LoadShaderAndCompile(std::string _path, GLenum _type) -> void;
	auto	LinkShaders() -> void;
	
	auto	GetShaderAttrib(std::string _name) -> GLint;
	auto	GetUniform(std::string _name) -> GLint;

	auto	GetProgram() -> GLuint { return m_ShaderResources[PROGRAM]; }

	auto	operator = (const OGL_Shader&)->OGL_Shader& = delete;
	auto	operator = (OGL_Shader&&)->OGL_Shader& = delete;

private:
	GLuint m_ShaderResources[RESOURCES_COUNT] = { 0 };

};


#endif /*__OGL_SHADER_HPP__*/