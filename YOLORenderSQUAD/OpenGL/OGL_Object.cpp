#include "OGL_Object.h"

#include <iostream>
#include <fstream>
#include <chrono>

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif
#include <stb-master\stb_image.h>

#define FREEGLUT_LIB_PRAGMAS 0
#include <freeglut\include\GL\freeglut.h>

#define BUFFER_OFFSET(offset) ((GLvoid*)(offset))
#define PI 3.14159265359

#include "Matrix.hpp"

using namespace std;

struct CommonMatrices
{
	float m_ProjectionMatrix[16];
	float m_ViewMatrix[16];
	float m_WorldMatrix[16];
};


OGL_Object::OGL_Object()
{
}


auto
OGL_Object::FromMeshData(const MeshData& _source) -> void
{
	m_polyCount = _source.m_PolyCount;
	m_verticesSize = _source.m_VertexSize;
	m_verticesCount = _source.m_VerticesCount;
	m_indices = new GLuint[m_IndicesCount()];
	m_data = new GLfloat[_source.m_VertexSize * _source.m_VerticesCount];
	SetAttribSizes((GLint*)_source.m_AttribSizes.data(), _source.m_AttribSizes.size());

	memcpy(m_indices, _source.m_Indices.data(), m_IndicesCount() * sizeof(unsigned int));

	int i = 0;
	for (auto point : _source.m_Points)
	{
		memcpy(m_data + i * m_verticesSize, point.m_Position, m_verticesSize * sizeof(float));
		++i;
	}
}


OGL_Object::~OGL_Object()
{
	FreeResources();

	if (m_data) delete[] m_data;
	if (m_indices) delete[] m_indices;
	if (m_attribSizes) delete[] m_attribSizes;
}


auto
OGL_Object::Initialize() -> void
{
	glGenBuffers(1, &m_CommonMatricesUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, m_CommonMatricesUBO);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(CommonMatrices), nullptr, GL_STREAM_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}


auto
OGL_Object::Update(float _deltaTime) -> void
{
	GLfloat rotationX[] = { 1.f, 0.f, 0.f, 0.f,
							0.f, cos(_deltaTime), sin(_deltaTime), 0.f,
							0.f, -sin(_deltaTime), cos(_deltaTime), 0.f,
							0.f, 0.f, 0.f, 1.f };
	Matrix::identity(rotationX);

	GLfloat rotationY[] = { cos(_deltaTime), 0.f, -sin(_deltaTime), 0.f,
								 0.f, 1.f, 0.f, 0.f,
								 sin(_deltaTime), 0.f, cos(_deltaTime), 0.f,
								 0.f, 0.f, 0.f, 1.f };

	GLfloat rotationZ[] = { cos(_deltaTime), sin(_deltaTime), 0.f, 0.f,
							-sin(_deltaTime), cos(_deltaTime), 0.f, 0.f,
							0.f, 0.f, 1.f, 0.f,
							0.f, 0.f, 0.f, 1.f };

	GLfloat translationMatrix[] = { 1.f, 0.f, 0.f, 0.f,
									0.f, 1.f, 0.f, 0.f,
									0.f, 0.f, 1.f, 0.f,
									0.f, 0.f, 0.f, 1.f };

	GLfloat scaleMatrix[] = { 1.f, 0.f, 0.f, 0.f,
							  0.f, 1.f, 0.f, 0.f,
							  0.f, 0.f, 1.f, 0.f,
							  0.f, 0.f, 0.f, 1.f };

	GLfloat rotationMatrix[16];
	Matrix::mul(rotationZ, rotationX, rotationMatrix);
	Matrix::mul(rotationMatrix, rotationY, rotationMatrix);

	GLfloat worldMatrix[16];
	Matrix::mul(translationMatrix, rotationMatrix, worldMatrix);
	Matrix::mul(worldMatrix, scaleMatrix, worldMatrix);

	static GLfloat projectionMatrix[16];
	//ComputeOrthographicProjection(projectionMatrix, -1.f, 1.f, -1.f, 1.f, 5.f, -1.f);
	//ComputePerspectiveProjection(projectionMatrix, -1.f, 1.f, -1.f, 1.f, 1.f, -1.f);
	ComputePerspectiveProjectionFOV(projectionMatrix, 90.f, (GLfloat)glutGet(GLUT_SCREEN_WIDTH) / glutGet(GLUT_SCREEN_HEIGHT), .1f, 1000.f);

	GLfloat cameraMatrix[] = { 1.f, 0.f, 0.f, 0.f,
							   0.f, 1.f, 0.f, 0.f,
							   0.f, 0.f, 1.f, 0.f,
							   0.f, -2.f, -5.f, 1.f };

	glBindBuffer(GL_UNIFORM_BUFFER, m_CommonMatricesUBO);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, 16 * sizeof(GLfloat), projectionMatrix);
	glBufferSubData(GL_UNIFORM_BUFFER, 16 * sizeof(GLfloat), 16 * sizeof(GLfloat), cameraMatrix);
	glBufferSubData(GL_UNIFORM_BUFFER, 32 * sizeof(GLfloat), 16 * sizeof(GLfloat), worldMatrix);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	//glUniform1f(glGetUniformLocation(m_shaderProgram, "u_time"), _deltaTime);
	//glUniformMatrix4fv(glGetUniformLocation(m_shaderProgram, "u_worldRotationMatrix"), 1, GL_FALSE, rotationMatrix);
	//glUniformMatrix4fv(glGetUniformLocation(m_shaderProgram, "u_worldTranslationMatrix"), 1, GL_FALSE, translationMatrix);
	//glUniformMatrix4fv(glGetUniformLocation(m_shaderProgram, "u_worldScaleMatrix"), 1, GL_FALSE, scaleMatrix);
	//glUniformMatrix4fv(glGetUniformLocation(m_shaderProgram, "u_projectionMatrix"), 1, GL_FALSE, projectionMatrix);
	//glUniformMatrix4fv(glGetUniformLocation(m_shaderProgram, "u_cameraMatrix"), 1, GL_FALSE, cameraMatrix);
}


auto
OGL_Object::Render() -> void
{
	if (m_shaderProgram)
	{
		glUseProgram(m_shaderProgram);

		glUniformBlockBinding(m_shaderProgram, glGetUniformBlockIndex(m_shaderProgram, "matrices"), 0);
		glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_CommonMatricesUBO);
	}

	if (!m_vbo || !m_ibo)
	{
		for (GLint index = 0; index < m_attribCount; ++index)
		{
			int offset = m_ComputeVertexOffset(index);

			glVertexAttribPointer(index, m_attribSizes[index], GL_FLOAT, GL_FALSE, m_verticesSize * sizeof(GLfloat), m_data + offset);
			glEnableVertexAttribArray(index);
		}
		glDrawElements(GL_TRIANGLES, m_IndicesCount(), GL_UNSIGNED_INT, m_indices);

		for (GLint index = 0; index < m_attribCount; ++index)
		{
			glDisableVertexAttribArray(index);
		}
	}
	else if(!m_vao)
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);

		for (GLint index = 0; index < m_attribCount; ++index)
		{
			int offset = m_ComputeVertexOffset(index);

			glVertexAttribPointer(index, m_attribSizes[index], GL_FLOAT, GL_FALSE, m_verticesSize * sizeof(GLfloat), BUFFER_OFFSET(offset * sizeof(GLfloat)));
			glEnableVertexAttribArray(index);
		}

		glDrawElements(GL_TRIANGLES, m_DataCount(), GL_UNSIGNED_INT, nullptr);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	else
	{
		glBindVertexArray(m_vao);
		glDrawElements(GL_TRIANGLES, m_IndicesCount(), GL_UNSIGNED_INT, nullptr);
		glBindVertexArray(0);
	}
}


auto
OGL_Object::RenderInstanced(GLsizei _instancesCount) -> void
{
	if (m_shaderProgram && m_vao)
	{
		glUseProgram(m_shaderProgram);

		glUniformBlockBinding(m_shaderProgram, glGetUniformBlockIndex(m_shaderProgram, "matrices"), 0);
		glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_CommonMatricesUBO);

		glBindVertexArray(m_vao);
		//glDrawArraysInstanced(GL_TRIANGLES, 0, m_verticesCount, _instancesCount);
		glDrawElementsInstanced(GL_TRIANGLES, m_DataCount(), GL_UNSIGNED_INT, nullptr, _instancesCount);
		glBindVertexArray(0);
	}
}


auto
OGL_Object::BindFBO() -> void
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
}


auto
OGL_Object::BlitAndUnbindFBO() -> void
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	glBlitFramebuffer(0, 0, 800, 800, 0, 0, 800, 800, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
}


auto
OGL_Object::FreeResources() -> void
{
	if (m_vbo)
	{
		glDeleteBuffers(1, &m_vbo);
		m_vbo = 0;
	}

	if (m_ibo)
	{
		glDeleteBuffers(1, &m_ibo);
		m_ibo = 0;
	}

	if (m_vao)
	{
		glDeleteVertexArrays(1, &m_vao);
		m_vao = 0;
	}

	if (m_fbo)
	{
		glDeleteFramebuffers(1, &m_fbo);
		m_fbo = 0;
	}

	if (m_CommonMatricesUBO)
	{
		glDeleteBuffers(1, &m_CommonMatricesUBO);
		m_CommonMatricesUBO = 0;
	}

	if (m_shaderProgram)
	{
		glDeleteProgram(m_shaderProgram);
		m_shaderProgram = 0;
	}
}


auto
OGL_Object::LoadShaderAndCompile(string _path, GLenum _type) -> void
{
	fstream sourceFile(_path, fstream::in);
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
		cout << log << endl;
	}

	if (_type == GL_VERTEX_SHADER)
		m_vertexShader = shader;
	else if (_type == GL_FRAGMENT_SHADER)
		m_fragmentShader = shader;
}


auto
OGL_Object::LinkShaders() -> void
{
	m_shaderProgram = glCreateProgram();

	if (m_vertexShader) glAttachShader(m_shaderProgram, m_vertexShader);
	if (m_fragmentShader) glAttachShader(m_shaderProgram, m_fragmentShader);
	glLinkProgram(m_shaderProgram);

	GLint linkingFailed = 0;
	glGetProgramiv(m_shaderProgram, GL_LINK_STATUS, &linkingFailed);

	if (linkingFailed == GL_FALSE)
	{
		GLint size = 0;
		GLint returnedSize = 0;
		glGetProgramiv(m_shaderProgram, GL_INFO_LOG_LENGTH, &size);

		char* log = new char[size];
		glGetProgramInfoLog(m_shaderProgram, size, &returnedSize, log);
		cout << log << endl;
	}

	glValidateProgram(m_shaderProgram);
}


auto
OGL_Object::GetShaderAttrib(string _attribName) -> GLint
{
	return glGetAttribLocation(m_shaderProgram, _attribName.c_str());
}


auto
OGL_Object::CreateTexture(const std::string& _path, bool _forceAlpha) -> GLuint
{
	GLuint texID = 0;

	int w, h, d;
	auto* data = stbi_load(_path.c_str(), &w, &h, &d, _forceAlpha ? STBI_rgb_alpha : STBI_rgb);

	if (data)
	{
		glGenTextures(1, &texID);
		glBindTexture(GL_TEXTURE_2D, texID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, _forceAlpha ? GL_RGBA : GL_RGB, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		//glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, w, h);
		//glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, data);

		glGenerateMipmap(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, 0);

		stbi_image_free(data);
	}

	glGenSamplers(1, &m_Sampler);
	glSamplerParameteri(m_Sampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);

	return texID;
}


auto
OGL_Object::EnableTexture(GLuint _index) -> void
{
	int i = 0;
	glBindSampler(i, m_Sampler);
	glActiveTexture(GL_TEXTURE0 + i);
	glBindTexture(GL_TEXTURE_2D, _index);
}


auto
OGL_Object::CreateVAO() -> void
{
	glGenVertexArrays(1, &m_vao);

	glBindVertexArray(m_vao);
	
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);

	for (GLint index = 0; index < m_attribCount; ++index)
	{
		int offset = m_ComputeVertexOffset(index);

		glVertexAttribPointer(index, m_attribSizes[index], GL_FLOAT, GL_FALSE, m_verticesSize * sizeof(GLfloat), BUFFER_OFFSET(offset * sizeof(GLfloat)));
		glEnableVertexAttribArray(index);
	}

	glBindVertexArray(0);
}


auto
OGL_Object::CreateVBO() -> void
{
	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, m_verticesSize * m_verticesCount * sizeof(GLfloat), m_data, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}


auto
OGL_Object::CreateIBO() -> void
{
	glGenBuffers(1, &m_ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_IndicesCount() * sizeof(GLuint), m_indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}


auto
OGL_Object::CreateFBO(int _width, int _height) -> void
{
	glGenFramebuffers(1, &m_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

	glGenTextures(1, &m_Framebuffer[COLOR_BUFFER]);
	
	glBindTexture(GL_TEXTURE_2D, m_Framebuffer[COLOR_BUFFER]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _width, _height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenRenderbuffers(1, &m_Framebuffer[DEPTH_BUFFER]);
	glBindRenderbuffer(GL_RENDERBUFFER, m_Framebuffer[DEPTH_BUFFER]);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, _width, _height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_Framebuffer[COLOR_BUFFER], 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_Framebuffer[DEPTH_BUFFER]);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		printf("TRISTESSE ULTIME\n");
	else
		printf("SUCCESS\n");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


auto
OGL_Object::DumpData() -> void
{
	for (int i = 0; i < m_verticesCount; ++i)
	{
		for (int j = 0; j < m_verticesSize; ++j)
		{
			cout << m_data[i * m_verticesSize + j] << "; ";
		}
		cout << endl;
	}
}


auto
OGL_Object::SetData(GLfloat* _data, int _count) -> void
{
	if (m_data) delete[] m_data;

	m_data = new GLfloat[_count];
	memcpy(m_data, _data, _count * sizeof(GLfloat));
}


auto
OGL_Object::SetIndices(GLuint* _indices, int _count) -> void
{
	if (m_indices) delete[] m_indices;

	m_indices = new GLuint[_count];
	memcpy(m_indices, _indices, _count * sizeof(GLuint));
}


auto
OGL_Object::SetAttribSizes(GLint* _attribSizes, GLint _count) -> void
{
	if (m_attribSizes) delete[] m_attribSizes;

	m_attribCount = _count;
	m_attribSizes = new GLint[_count];
	memcpy(m_attribSizes, _attribSizes, _count * sizeof(GLint));
}


auto
OGL_Object::SetPolyCount(int _count) -> void
{
	m_polyCount = _count;
}


auto
OGL_Object::SetVerticesSize(int _size) -> void
{
	m_verticesSize = _size;
}


auto
OGL_Object::SetVerticesCount(int _count) -> void
{
	m_verticesCount = _count;
}


auto	
OGL_Object::ComputeOrthographicProjection(GLfloat* o_matrix, GLfloat _L, GLfloat _R, GLfloat _B, GLfloat _T, GLfloat _F, GLfloat _N) -> void
{
	GLfloat result[] =
	{
		(2.f / (_R - _L)), 0.f, 0.f, -((_R + _L) / (_R - _L)),
		0.f, (2.f / (_T - _B)), 0.f, -((_T + _B) / (_T - _B)),
		0.f, 0.f, (-2.f / (_F - _N)), -((_F + _N) / (_F - _N)),
		0.f, 0.f, 0.f, 1.f
	};

	memcpy(o_matrix, result, 16 * sizeof(GLfloat));
}


auto
OGL_Object::ComputePerspectiveProjection(GLfloat* o_matrix, GLfloat _L, GLfloat _R, GLfloat _B, GLfloat _T, GLfloat _F, GLfloat _N) -> void
{
	GLfloat result[] =
	{
		((-2.f * _N) / (_R - _L)), 0.f, ((_R + _L) / (_R - _L)), 0.f,
		0.f, ((-2.f * _N) / (_T - _B)), ((_T + _B) / (_T - _B)), 0.f,
		0.f, 0.f, (-(_F + _N) / (_F - _N)), (-2.f * _F * _N) / (_F - _N),
		0.f, 0.f, -1.f, 0.f
	};

	memcpy(o_matrix, result, 16 * sizeof(GLfloat));
}


auto
OGL_Object::ComputePerspectiveProjectionFOV(GLfloat* o_matrix, GLfloat _fov, GLfloat _aspectRatio, GLfloat _near, GLfloat _far) -> void
{
	GLfloat xymax = _near * (GLfloat)tan(_fov * (PI / 360.));
	GLfloat width = 2 * xymax;

	GLfloat depth = _far - _near;
	GLfloat q = -(_far + _near) / depth;
	GLfloat qn = -2 * (_far * _near) / depth;

	GLfloat w = (2 * _near / width) / _aspectRatio;
	GLfloat h = 2 * _near / width;

	GLfloat result[] =
	{
		w, 0.f, 0.f, 0.f,
		0.f, h, 0.f, 0.f,
		0.f, 0.f, q, -1.f,
		0.f, 0.f, qn, 0.f
	};

	memcpy(o_matrix, result, 16 * sizeof(GLfloat));
}


auto
OGL_Object::m_ComputeVertexOffset(GLint _index) -> int
{
	int offset = 0;
	if (_index > 0)
	{
		for (int i = _index - 1; i >= 0; --i)
		{
			offset += m_attribSizes[i];
		}
	}

	return offset;
}