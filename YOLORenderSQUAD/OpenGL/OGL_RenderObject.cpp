#include "OGL_RenderObject.hpp"


void
OGL_RenderObject::Render()
{
	for (std::vector<OGL_Mesh>::iterator ite = m_Meshes.begin(); ite != m_Meshes.end(); ++ite)
	{
		OGL_Shader* shader = ite->GetShader();
		if (shader)
		{
			shader->EnableShader();
			glUniformMatrix4fv(shader->GetUniform("u_WorldMatrix"), 1, GL_FALSE, getNode()->WorldTransform().GetMatrix().data);
		}

		ite->Render(true);
	}
}


void
OGL_RenderObject::AddMesh(const MeshData& _source, OGL_Shader* _shader)
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


void
OGL_RenderObject::AddMultiMesh(const MultiMeshData& _source, OGL_Shader* _shader)
{
	for (size_t i = 0; i < _source.m_Meshes.size(); ++i)
	{
		auto& meshData = _source.m_Meshes[i];
		AddMesh(meshData, _shader);
	}
}

