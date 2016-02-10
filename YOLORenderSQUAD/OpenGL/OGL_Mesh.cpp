#include "OGL_Mesh.hpp"

#include <fstream>

#include <stb-master\stb_image.h>

#define BUFFER_OFFSET(offset) ((GLvoid*)(offset))


OGL_Mesh::OGL_Mesh(const MeshData& _source)
{
	m_PolyCount = _source.m_PolyCount;
	m_VertexSize = _source.m_VertexSize;
	m_VerticesCount = _source.m_VerticesCount;
	m_Indices = new GLuint[m_IndicesCount()];
	m_Data = new GLfloat[_source.m_VertexSize * _source.m_VerticesCount];
	SetAttribSizes((GLint*)_source.m_AttribSizes.data(), _source.m_AttribSizes.size());

	memcpy(m_Indices, _source.m_Indices.data(), _source.m_Indices.size() * sizeof(unsigned int));
	
	for (int i = 0; i < _source.m_Points.size(); ++i)
	{
		auto& point = _source.m_Points[i];
		memcpy(m_Data + i * m_VertexSize, point.m_Position, m_VertexSize * sizeof(float));
	}

	FillBuffers();

	m_Folder = _source.m_Folder;
	m_Material = _source.m_Material;
	if (m_Material.map_Ka != "")
		CreateTexture(m_Folder + m_Material.map_Ka);
}


OGL_Mesh::OGL_Mesh(const OGL_Mesh& _other)
{
	m_PolyCount = _other.m_PolyCount;
	m_VertexSize = _other.m_VertexSize;
	m_VerticesCount = _other.m_VerticesCount;
	m_Indices = new GLuint[m_IndicesCount()];
	m_Data = new GLfloat[m_VertexSize * m_VerticesCount];
	SetAttribSizes(_other.m_VertexAttributesSizes, _other.m_VertexAttributesCount);

	if (_other.m_Indices)
		memcpy(m_Indices, _other.m_Indices, _other.m_IndicesCount() * sizeof(GLuint));
	if (_other.m_Data)
		memcpy(m_Data, _other.m_Data, m_VertexSize * m_VerticesCount * sizeof(GLfloat));

	m_Folder = _other.m_Folder;
	m_Material = _other.m_Material;
	if (m_Material.map_Ka != "")
		CreateTexture(m_Folder + m_Material.map_Ka);

	m_HasSampler = _other.m_HasSampler;
	m_Sampler = _other.m_Sampler;

	m_Shader = _other.m_Shader;

	FillBuffers();
}


OGL_Mesh::OGL_Mesh(OGL_Mesh&& _other)
{
	m_PolyCount = _other.m_PolyCount; _other.m_PolyCount = 0;
	m_VertexSize = _other.m_VertexSize; _other.m_VertexSize = 0;
	m_VerticesCount = _other.m_VerticesCount; _other.m_VerticesCount = 0;

	m_Indices = _other.m_Indices; _other.m_Indices = nullptr;
	m_Data = _other.m_Data; _other.m_Data = nullptr;

	m_VertexAttributesCount = _other.m_VertexAttributesCount;
	_other.m_VertexAttributesCount = 0;
	m_VertexAttributesSizes = _other.m_VertexAttributesSizes;
	_other.m_VertexAttributesSizes = nullptr;

	m_Folder = _other.m_Folder;
	m_Material = _other.m_Material;
	m_TexturesCount = _other.m_TexturesCount; _other.m_TexturesCount = 0;
	m_Textures = _other.m_Textures; _other.m_Textures = nullptr;

	m_HasSampler = _other.m_HasSampler; _other.m_HasSampler = false;
	m_Sampler = _other.m_Sampler; _other.m_Sampler = 0;

	m_Shader = _other.m_Shader; _other.m_Shader = nullptr;

	m_Initialized = _other.m_Initialized; _other.m_Initialized = false;
	for (int i = 0; i < BUFFERS_COUNT; ++i)
	{
		m_BufferObjects[i] = _other.m_BufferObjects[i];
		_other.m_BufferObjects[i] = 0;
	}
}


OGL_Mesh::~OGL_Mesh()
{
	FreeOGLResources();

	if (m_Data) delete[] m_Data;
	if (m_Indices) delete[] m_Indices;
	if (m_VertexAttributesSizes) delete[] m_VertexAttributesSizes;
	if (m_Textures) delete[] m_Textures;
}


auto
OGL_Mesh::Render() -> void
{
	if (!m_Initialized) return;

	if (m_TexturesCount > 0)
	{
		for (GLuint index = 0; index < m_TexturesCount; ++index)
		{
			glBindSampler(index, m_Sampler);
			glActiveTexture(GL_TEXTURE0 + index);
			glBindTexture(GL_TEXTURE_2D, m_Textures[index]);
		}
	}

	glBindVertexArray(m_BufferObjects[VAO]);
	glDrawElements(GL_TRIANGLES, m_DataCount(), GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);
}


auto
OGL_Mesh::Render(GLuint _pvMatricesBuffer) -> void
{
	if (!m_Initialized) return;

	if (m_Shader)
	{
		m_Shader->EnableShader();

		glUniformBlockBinding(m_Shader->GetProgram(), glGetUniformBlockIndex(m_Shader->GetProgram(), "pvMatrices"), 42);
		glBindBufferBase(GL_UNIFORM_BUFFER, 42, _pvMatricesBuffer);

		glUniformMatrix4fv(m_Shader->GetUniform("u_WorldMatrix"), 1, GL_FALSE, m_Transform.GetMatrix().data);
	}

	if (m_TexturesCount > 0)
	{
		for (GLuint index = 0; index < m_TexturesCount; ++index)
		{
			glBindSampler(index, m_Sampler);
			glActiveTexture(GL_TEXTURE0 + index);
			glBindTexture(GL_TEXTURE_2D, m_Textures[index]);
		}
	}

	glBindVertexArray(m_BufferObjects[VAO]);
	glDrawElements(GL_TRIANGLES, m_DataCount(), GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);
}


auto
OGL_Mesh::FillBuffers() -> void
{
	if (!m_Initialized)
	{
		glGenVertexArrays(1, &m_BufferObjects[VAO]);
		glGenBuffers(3, &m_BufferObjects[VBO]);

		// VBO
		glBindBuffer(GL_ARRAY_BUFFER, m_BufferObjects[VBO]);
		glBufferData(GL_ARRAY_BUFFER, m_VertexSize * m_VerticesCount * sizeof(GLfloat), m_Data, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// IBO
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BufferObjects[IBO]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_IndicesCount() * sizeof(GLuint), m_Indices, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		// VAO
		glBindVertexArray(m_BufferObjects[VAO]);
	
		glBindBuffer(GL_ARRAY_BUFFER, m_BufferObjects[VBO]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BufferObjects[IBO]);

		for (GLint index = 0; index < m_VertexAttributesCount; ++index)
		{
			int offset = m_ComputeVertexOffset(index);

			glVertexAttribPointer(index, m_VertexAttributesSizes[index], GL_FLOAT, GL_FALSE, m_VertexSize * sizeof(GLfloat), BUFFER_OFFSET(offset * sizeof(GLfloat)));
			glEnableVertexAttribArray(index);
		}

		glBindVertexArray(0);
	
		m_Initialized = true;
	}
}


auto
OGL_Mesh::FreeOGLResources() -> void
{
	if (m_Initialized)
	{
		glDeleteVertexArrays(1, &m_BufferObjects[VAO]);
		glDeleteBuffers(3, &m_BufferObjects[VBO]);
		glDeleteTextures(m_TexturesCount, m_Textures);
		if (m_HasSampler) glDeleteSamplers(1, &m_Sampler);

		m_Initialized = false;
	}
}


auto
OGL_Mesh::CreateTexture(const std::string& _path, bool _forceAlpha) -> void
{
	if (!std::fstream(_path).good()) return;

	int w, h, d;
	auto* data = stbi_load(_path.c_str(), &w, &h, &d, _forceAlpha ? STBI_rgb_alpha : STBI_rgb);

	if (data)
	{
		GLuint* textures = new GLuint[m_TexturesCount + 1];
		memcpy(textures, m_Textures, m_TexturesCount);
		delete[] m_Textures;
		m_Textures = textures;

		glGenTextures(1, &m_Textures[m_TexturesCount]);
		glBindTexture(GL_TEXTURE_2D, m_Textures[m_TexturesCount]);
		glTexImage2D(GL_TEXTURE_2D, 0, _forceAlpha ? GL_RGBA : GL_RGB, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);

		stbi_image_free(data);
		m_TexturesCount++;
	}

	if (!m_HasSampler)
	{
		glGenSamplers(1, &m_Sampler);
		glSamplerParameteri(m_Sampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		glSamplerParameteri(m_Sampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	
		m_HasSampler = true;
	}
}


auto
OGL_Mesh::SetData(GLfloat* _data) -> void
{
	if (m_Data) delete[] m_Data;

	int dataCount = m_VertexSize * m_VerticesCount;
	m_Data = new GLfloat[dataCount];
	memcpy(m_Data, _data, dataCount * sizeof(GLfloat));
}


auto
OGL_Mesh::SetIndices(GLuint* _indices) -> void
{
	if (m_Indices) delete[] m_Indices;

	m_Indices = new GLuint[m_IndicesCount()];
	memcpy(m_Indices, _indices, m_IndicesCount() * sizeof(GLuint));
}


auto
OGL_Mesh::SetAttribSizes(GLint* _attribSizes, GLint _count) -> void
{
	if (m_VertexAttributesSizes) delete[] m_VertexAttributesSizes;

	m_VertexAttributesCount = _count;
	m_VertexAttributesSizes = new GLint[_count];
	memcpy(m_VertexAttributesSizes, _attribSizes, _count * sizeof(GLint));
}


auto
OGL_Mesh::m_ComputeVertexOffset(GLint _index) const -> int
{
	int offset = 0;
	if (_index > 0)
	{
		for (int i = _index - 1; i >= 0; --i)
		{
			offset += m_VertexAttributesSizes[i];
		}
	}

	return offset;
}
