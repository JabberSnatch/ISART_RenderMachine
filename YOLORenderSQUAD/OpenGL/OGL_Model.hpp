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
	auto	GetMesh(int i) -> OGL_Mesh& { return m_Meshes[i]; }

	auto	operator = (const OGL_Model&)->OGL_Model& = delete;
	auto	operator = (OGL_Model&&)->OGL_Model& = delete;

private:
	Transform					m_Transform;

	std::vector<OGL_Mesh>		m_Meshes;

};


#endif /*__OGL_MODEL_HPP__*/