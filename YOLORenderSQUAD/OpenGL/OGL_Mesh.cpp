#include "OGL_Mesh.hpp"

#include <fstream>

#include <stb-master\stb_image.h>

#include "MaterialData.hpp"
#include "OGL_TextureLoader.hpp"

#define BUFFER_OFFSET(offset) ((GLvoid*)(offset))

using MD = MaterialData;


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
	for (int id = 0; id < MD::TEX_ID_COUNT; ++id)
		if (m_Material.tex_maps[id] != "")
			CreateTexture(m_Folder + m_Material.tex_maps[id], (MD::TEXTURE_ID)id);
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
	for (int id = 0; id < MD::TEX_ID_COUNT; ++id)
		if (m_Material.tex_maps[id] != "")
			CreateTexture(m_Folder + m_Material.tex_maps[id], (MD::TEXTURE_ID)id);

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

	memcpy(m_Textures, _other.m_Textures, MaterialData::TEX_ID_COUNT * sizeof(GLuint));
	memset(_other.m_Textures, 0, MaterialData::TEX_ID_COUNT * sizeof(GLuint));

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
}


auto
OGL_Mesh::Render(bool _shaderEnabled) -> void
{
	if (!m_Initialized) return;

	if (m_Shader)
	{
		glUniform3fv(m_Shader->GetUniform("IN_MATERIAL.Ka"), 1, m_Material.Ka);
		glUniform3fv(m_Shader->GetUniform("IN_MATERIAL.Kd"), 1, m_Material.Kd);
		glUniform3fv(m_Shader->GetUniform("IN_MATERIAL.Ks"), 1, m_Material.Ks);
		glUniform1f(m_Shader->GetUniform("IN_MATERIAL.illum"), m_Material.illum);
		glUniform1f(m_Shader->GetUniform("IN_MATERIAL.d"), m_Material.d);
		glUniform1f(m_Shader->GetUniform("IN_MATERIAL.Ns"), m_Material.Ns);
		glUniform1f(m_Shader->GetUniform("IN_MATERIAL.sharpness"), m_Material.sharpness);
		glUniform1f(m_Shader->GetUniform("IN_MATERIAL.Ni"), m_Material.Ni);

		for (int index = 0; index < MD::TEX_ID_COUNT; ++index)
		{
			std::string uniformName = "u_" + MD::TexIDToString((MD::TEXTURE_ID)index);
			if (m_Textures[index] != 0)
			{
				glBindSampler(index, m_Sampler);
				glActiveTexture(GL_TEXTURE0 + index);
				glBindTexture(GL_TEXTURE_2D, m_Textures[index]);
				glUniform1i(m_Shader->GetUniform(uniformName), index);
				glUniform1i(m_Shader->GetUniform(uniformName + "_bound"), 1);
			}
			else
				glUniform1i(m_Shader->GetUniform(uniformName + "_bound"), 0);
		}

		if (!_shaderEnabled) m_Shader->EnableShader();
	}

	glBindVertexArray(m_BufferObjects[VAO]);
	glDrawElements(GL_TRIANGLES, m_IndicesCount(), GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);

	for (int index = 0; index < MD::TEX_ID_COUNT; ++index)
	{
		if (m_Textures[index] != 0)
		{
			glBindSampler(index, 0);
			glActiveTexture(GL_TEXTURE0 + index);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
	}
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

	Render(true);
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
		if (m_HasSampler) glDeleteSamplers(1, &m_Sampler);

		m_Initialized = false;
	}
}


auto
OGL_Mesh::CreateTexture(const std::string& _path, MD::TEXTURE_ID _id, bool _forceAlpha) -> void
{
	if (!std::fstream(_path).good()) return;

	m_Textures[_id] = OGL_TextureLoader::Get()->GetTexture(_path, _forceAlpha);
	if (0)
	{
		int w, h, d;
		auto* data = stbi_load(_path.c_str(), &w, &h, &d, _forceAlpha ? STBI_rgb_alpha : STBI_rgb);

		if (data)
		{
			glGenTextures(1, &m_Textures[_id]);
			glBindTexture(GL_TEXTURE_2D, m_Textures[_id]);
			glTexImage2D(GL_TEXTURE_2D, 0, _forceAlpha ? GL_RGBA : GL_RGB, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, 0);

			stbi_image_free(data);
		}
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
