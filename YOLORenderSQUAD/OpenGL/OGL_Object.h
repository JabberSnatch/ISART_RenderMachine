#pragma once

#include <glew/include/GL/glew.h>

#include <string>

#include "ObjParser.hpp"

class OGL_Object
{
	enum FRAMEBUFFER_ID
	{
		COLOR_BUFFER = 0,
		DEPTH_BUFFER,
		STENCIL_BUFFER,

		BUFFERS_COUNT
	};

public:
	OGL_Object();
	//OGL_Object(const MeshData& _source);
	~OGL_Object();

	auto	Initialize() -> void;
	auto	FromMeshData(const MeshData& _source) -> void;

	auto	Update(float _deltaTime) -> void;
	auto	Render() -> void;
	auto	RenderInstanced(int _instancesCount) -> void;

	auto	BindFBO() -> void;
	auto	BlitAndUnbindFBO() -> void;

	auto	FreeResources() -> void;

	auto	LoadShaderAndCompile(std::string _path, GLenum _type) -> void;
	auto	LinkShaders() -> void;
	auto	GetShaderAttrib(std::string _attribName) -> GLint;

	auto	CreateTexture(const std::string& _path, bool _forceAlpha = true) -> GLuint;
	auto	EnableTexture(GLuint _index) -> void;

	auto	CreateVAO() -> void;
	auto	CreateVBO() -> void;
	auto	CreateIBO() -> void;
	auto	CreateFBO(int _weight, int _height) -> void;

	auto	DumpData() -> void;

	auto	SetData(GLfloat* _data, int _count) -> void;
	auto	SetIndices(GLuint* _indices, int _count) -> void;
	auto	SetAttribSizes(GLint* _attribSizes, GLint _count) -> void;
	auto	SetPolyCount(int _count) -> void;
	auto	SetVerticesSize(int _size) -> void;
	auto	SetVerticesCount(int _count) -> void;

	static auto	ComputeOrthographicProjection(GLfloat* o_matrix, GLfloat _L, GLfloat _R, GLfloat _B, GLfloat _T, GLfloat _F, GLfloat _N) -> void;
	static auto	ComputePerspectiveProjection(GLfloat* o_matrix, GLfloat _L, GLfloat _R, GLfloat _B, GLfloat _T, GLfloat _F, GLfloat _N) -> void;
	static auto	ComputePerspectiveProjectionFOV(GLfloat* o_matrix, GLfloat _fov, GLfloat _ratio, GLfloat _near, GLfloat _far) -> void;

private:
	auto	m_ComputeVertexOffset(GLint _index) -> int;
	auto	m_IndicesCount() -> int { return m_polyCount * 3; }
	auto	m_DataCount() -> int { return m_IndicesCount() * m_verticesSize; }

	int m_polyCount = 0;
	int m_verticesCount = 0;
	int m_verticesSize = 0;

	GLfloat* m_data = nullptr;
	GLuint* m_indices = nullptr;

	GLint m_attribCount;
	GLint* m_attribSizes = nullptr;

	GLuint m_vao = 0;
	GLuint m_vbo = 0;
	GLuint m_ibo = 0;
	GLuint m_CommonMatricesUBO = 0;
	
	GLuint m_fbo = 0;
	GLuint m_Framebuffer[BUFFERS_COUNT];

	GLuint m_Sampler = 0;

	GLuint m_shaderProgram = 0;
	GLuint m_vertexShader = 0;
	GLuint m_fragmentShader = 0;
};