#include "OGL_Model.hpp"


auto	
OGL_Model::Render(GLuint _pvMatricesBuffer) -> void
{
	for (auto&& mesh : m_Meshes)
	{
		if(0)
		{
			OGL_Shader* normalShader = m_GetNormalsShader();
			normalShader->EnableShader();
			glUniformBlockBinding(normalShader->GetProgram(), glGetUniformBlockIndex(normalShader->GetProgram(), "pvMatrices"), 42);
			glBindBufferBase(GL_UNIFORM_BUFFER, 42, _pvMatricesBuffer);
			glUniformMatrix4fv(normalShader->GetUniform("u_WorldMatrix"), 1, GL_FALSE, m_Transform.GetMatrix().data);
			mesh.Render(true);
		}


		OGL_Shader* shader = mesh.GetShader();
		if (shader)
			glUniformMatrix4fv(shader->GetUniform("u_WorldMatrix"), 1, GL_FALSE, m_Transform.GetMatrix().data);

		mesh.Render();
	}
}


auto	
OGL_Model::AddMesh(const MeshData& _source, OGL_Shader* _shader) -> void
{
	OGL_Mesh mesh(_source);
	mesh.SetShader(_shader);
	m_Meshes.push_back(mesh);

	for (Point const& point : _source.m_Points)
	{
		if (m_Min.x > point[Point::POSITION][0])
			m_Min.x = point[Point::POSITION][0];
		if (m_Min.y > point[Point::POSITION][1])
			m_Min.y = point[Point::POSITION][1];
		if (m_Min.z > point[Point::POSITION][2])
			m_Min.z = point[Point::POSITION][2];

		if (m_Max.x < point[Point::POSITION][0])
			m_Max.x = point[Point::POSITION][0];
		if (m_Max.y < point[Point::POSITION][1])
			m_Max.y = point[Point::POSITION][1];
		if (m_Max.z < point[Point::POSITION][2])
			m_Max.z = point[Point::POSITION][2];
	}
}


auto	
OGL_Model::AddMultiMesh(const MultiMeshData& _source, OGL_Shader* _shader) -> void
{
	for (size_t i = 0; i < _source.m_Meshes.size(); ++i)
	{
		auto& meshData = _source.m_Meshes[i];
		AddMesh(meshData, _shader);
	}
}


OGL_Shader* OGL_Model::m_NormalsShader = nullptr;
auto
OGL_Model::m_GetNormalsShader() -> OGL_Shader*
{
	if (!m_NormalsShader)
	{
		m_NormalsShader = new OGL_Shader();
		m_NormalsShader->LoadShaderAndCompile("DrawNormals.vs", GL_VERTEX_SHADER);
		m_NormalsShader->LoadShaderAndCompile("DrawNormals.fs", GL_FRAGMENT_SHADER);
		m_NormalsShader->LoadShaderAndCompile("DrawNormals.gs", GL_GEOMETRY_SHADER);
		m_NormalsShader->LinkShaders();
	}
	return m_NormalsShader;
}