#ifndef __OGL_INSTANCED_MESH_HPP__
#define __OGL_INSTANCED_MESH_HPP__

#include "OGL_Mesh.hpp"


class OGL_InstancedMesh
{
public:
	OGL_InstancedMesh() = delete;
	OGL_InstancedMesh(const OGL_InstancedMesh&) = delete;
	OGL_InstancedMesh(OGL_InstancedMesh&&) = delete;
	~OGL_InstancedMesh() = delete;

	auto	operator = (const OGL_InstancedMesh&) -> OGL_InstancedMesh& = delete;
	auto	operator = (OGL_InstancedMesh&&) -> OGL_InstancedMesh& = delete;

private:
	OGL_Mesh*	m_SourceMesh;

	int			m_InstancesCount;
	GLfloat*	m_WorldMatrices;

};


#endif /*__OGL_INSTANCED_MESH_HPP__*/