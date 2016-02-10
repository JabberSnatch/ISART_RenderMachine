#include "OGL_Model.hpp"


auto	
OGL_Model::Render(GLuint _pvMatricesBuffer) -> void
{
	for (auto&& mesh : m_Meshes)
	{
		OGL_Shader* shader = &mesh.GetShader();
		shader->EnableShader();
		glUniformBlockBinding(shader->GetProgram(), glGetUniformBlockIndex(shader->GetProgram(), "pvMatrices"), 42);
		glBindBufferBase(GL_UNIFORM_BUFFER, 42, _pvMatricesBuffer);
		glUniformMatrix4fv(shader->GetUniform("u_WorldMatrix"), 1, GL_FALSE, m_Transform.GetMatrix().data);

		mesh.Render();
	}
}


auto	
OGL_Model::AddMesh(const MeshData& _source, OGL_Shader* _shader) -> void
{
	OGL_Mesh mesh(_source);
	mesh.SetShader(*_shader);
	m_Meshes.push_back(mesh);
}


auto	
OGL_Model::AddMultiMesh(const MultiMeshData& _source, OGL_Shader* _shader) -> void
{
	for (int i = 0; i < _source.m_Meshes.size(); ++i)
	{
		auto& meshData = _source.m_Meshes[i];
		AddMesh(meshData, _shader);
	}
}