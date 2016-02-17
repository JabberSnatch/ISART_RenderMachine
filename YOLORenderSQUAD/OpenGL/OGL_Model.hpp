#ifndef __OGL_MODEL_HPP__
#define __OGL_MODEL_HPP__

#include <vector>

#include "OGL_Mesh.hpp"
#include "Transform.hpp"


class OGL_Model
{
public:
	OGL_Model() = default;
	OGL_Model(const OGL_Model&) = delete;
	OGL_Model(OGL_Model&&) = delete;
	~OGL_Model() = default;

	auto	Render(GLuint _pvMatricesBuffer) -> void;
	auto	AddMesh(const MeshData& _source, OGL_Shader* _shader) -> void;
	auto	AddMultiMesh(const MultiMeshData& _source, OGL_Shader* _shader) -> void;

	auto	GetTransform() -> Transform& { return m_Transform; }
	auto	GetMin() -> Vec3 { return m_Min * m_Transform.Scale; }
	auto	GetMax() -> Vec3 { return m_Max * m_Transform.Scale; }
	auto	GetMesh(int i) -> OGL_Mesh& { return m_Meshes[i]; }
	auto	GetMeshes() -> std::vector<OGL_Mesh>& { return m_Meshes; }

	auto	operator = (const OGL_Model&)->OGL_Model& = delete;
	auto	operator = (OGL_Model&&)->OGL_Model& = delete;

private:
	Transform					m_Transform;
	Vec3						m_Min = Vec3::Zero();
	Vec3						m_Max = Vec3::Zero();

	std::vector<OGL_Mesh>		m_Meshes;


	static OGL_Shader* m_NormalsShader;
	static auto	m_GetNormalsShader() -> OGL_Shader*;

};


#endif /*__OGL_MODEL_HPP__*/