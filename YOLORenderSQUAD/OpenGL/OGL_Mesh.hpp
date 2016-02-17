#ifndef __OGL_MESH_HPP__
#define __OGL_MESH_HPP__

#include <glew/include/GL/glew.h>
#include <string>

#include "ObjParser.hpp"
#include "OGL_Shader.hpp"
#include "Transform.hpp"

class OGL_Mesh
{
	enum BUFFEROBJECTS
	{
		VAO = 0,
		VBO,
		IBO,
		WORLD_MATRIX,

		BUFFERS_COUNT
	};

public:
	OGL_Mesh() = default;
	OGL_Mesh(const MeshData&);
	OGL_Mesh(const OGL_Mesh&);
	OGL_Mesh(OGL_Mesh&&);
	~OGL_Mesh();

	auto	Render(bool _shaderEnabled = false) -> void;
	auto	Render(GLuint _pvMatricesBuffer) -> void;

	auto	FillBuffers() -> void;
	auto	FreeOGLResources() -> void;

	auto	CreateTexture(const std::string& _path, MaterialData::TEXTURE_ID _id, bool _forceAlpha = true) -> void;

	auto	GetTransform() -> Transform& { return m_Transform; }
	auto	GetShader() -> OGL_Shader* { return m_Shader; }

	auto	SetData(GLfloat* _data) -> void;
	auto	SetIndices(GLuint* _indices) -> void;
	auto	SetAttribSizes(GLint* _attribSizes, GLint _count) -> void;

	auto	SetPolyCount(int _count) -> void { m_PolyCount = _count; }
	auto	SetVerticesCount(int _count) -> void { m_VerticesCount = _count; }
	auto	SetVertexSize(int _size) -> void { m_VertexSize = _size; }

	auto	SetShader(OGL_Shader* _shader) -> void { m_Shader = _shader; }

	auto	operator = (const OGL_Mesh&)->OGL_Mesh& = delete;
	auto	operator = (OGL_Mesh&&)->OGL_Mesh& = delete;

private:
	auto	m_ComputeVertexOffset(GLint _index) const -> int;
	auto	m_IndicesCount() const -> int { return m_PolyCount * 3; }
	auto	m_DataCount() const -> int { return m_IndicesCount() * m_VertexSize; }

	bool		m_Initialized = false;

	Transform		m_Transform;
	MaterialData	m_Material;
	std::string		m_Folder;

	GLfloat*	m_Data = nullptr;
	GLuint*		m_Indices = nullptr;

	GLint		m_VertexAttributesCount = 0;
	GLint*		m_VertexAttributesSizes = nullptr;

	int			m_PolyCount = 0;
	int			m_VerticesCount = 0;
	int			m_VertexSize = 0;

	GLuint		m_Textures[MaterialData::TEX_ID_COUNT] = { 0 };

	bool		m_HasSampler = false;
	GLuint		m_Sampler = 0;

	GLuint		m_BufferObjects[BUFFERS_COUNT];

	OGL_Shader*	m_Shader;

};


#endif /*__OGL_MESH_HPP__*/