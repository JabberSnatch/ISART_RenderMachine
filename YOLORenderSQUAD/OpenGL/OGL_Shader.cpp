#include "OGL_Shader.hpp"

#include <fstream>
#include <iostream>


OGL_Shader::~OGL_Shader()
{
	if (m_ShaderResources[PROGRAM]) glDeleteProgram(m_ShaderResources[PROGRAM]);
	if (m_ShaderResources[VERTEX]) glDeleteShader(m_ShaderResources[VERTEX]);
	if (m_ShaderResources[FRAGMENT]) glDeleteShader(m_ShaderResources[FRAGMENT]);
	if (m_ShaderResources[GEOMETRY]) glDeleteShader(m_ShaderResources[GEOMETRY]);
}


auto
OGL_Shader::EnableShader() -> void
{
	if (!m_IsLinked)
		LinkShaders();

	glUseProgram(m_ShaderResources[PROGRAM]);
}


auto
OGL_Shader::LoadShaderAndCompile(std::string _path, GLenum _type) -> void
{
	std::fstream sourceFile(_path, std::fstream::in);
	sourceFile.seekg(0, sourceFile.end);
	std::streamoff length = sourceFile.tellg();
	sourceFile.seekg(0, sourceFile.beg);

	char* buffer = new char[(unsigned int)length];
	sourceFile.get(buffer, length, '\0');


	GLuint shader = glCreateShader(_type);

	const char* shaderString[1];
	shaderString[0] = buffer;

	glShaderSource(shader, 1, shaderString, nullptr);
	glCompileShader(shader);

	GLint result = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &result);

	if (result == GL_FALSE)
	{
		GLint size = 0;
		GLint returnedSize = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &size);

		char* log = new char[size];
		glGetShaderInfoLog(shader, size, &returnedSize, log);
		std::cout << log << std::endl;
	}

	if (_type == GL_VERTEX_SHADER)
		m_ShaderResources[VERTEX] = shader;
	else if (_type == GL_FRAGMENT_SHADER)
		m_ShaderResources[FRAGMENT] = shader;
	else if (_type == GL_GEOMETRY_SHADER)
		m_ShaderResources[GEOMETRY] = shader;
}


auto
OGL_Shader::LinkShaders() -> void
{
	if (!m_IsLinked)
	{
		m_ShaderResources[PROGRAM] = glCreateProgram();

		if (m_ShaderResources[VERTEX]) glAttachShader(m_ShaderResources[PROGRAM], m_ShaderResources[VERTEX]);
		if (m_ShaderResources[FRAGMENT]) glAttachShader(m_ShaderResources[PROGRAM], m_ShaderResources[FRAGMENT]);
		if (m_ShaderResources[GEOMETRY]) glAttachShader(m_ShaderResources[PROGRAM], m_ShaderResources[GEOMETRY]);
		glLinkProgram(m_ShaderResources[PROGRAM]);

		GLint linkingFailed = 0;
		glGetProgramiv(m_ShaderResources[PROGRAM], GL_LINK_STATUS, &linkingFailed);

		if (linkingFailed == GL_FALSE)
		{
			GLint size = 0;
			GLint returnedSize = 0;
			glGetProgramiv(m_ShaderResources[PROGRAM], GL_INFO_LOG_LENGTH, &size);

			char* log = new char[size];
			glGetProgramInfoLog(m_ShaderResources[PROGRAM], size, &returnedSize, log);
			std::cout << log << std::endl;
		}

		glValidateProgram(m_ShaderResources[PROGRAM]);
	}

	m_IsLinked = true;
}


auto
OGL_Shader::GetShaderAttrib(std::string _name) -> GLint
{
	if (!m_IsLinked)
		printf("Getting shader attrib on a unlinked program, please enable the shader.\n");

	return glGetAttribLocation(m_ShaderResources[PROGRAM], _name.c_str());
}


auto
OGL_Shader::GetUniform(std::string _name) -> GLint
{
	if (!m_IsLinked)
		printf("Getting shader uniform on a unlinked program, please enable the shader.\n");

	return glGetUniformLocation(m_ShaderResources[PROGRAM], _name.c_str());
}
