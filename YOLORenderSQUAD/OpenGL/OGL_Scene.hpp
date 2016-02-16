#ifndef __OGL_SCENE_HPP__
#define __OGL_SCENE_HPP__

#include <vector>

#include "OGL_Model.hpp"
#include "OGL_Mesh.hpp"
#include "OGL_InstancedMesh.hpp"


class OGL_Scene
{
	enum MATRICES
	{
		PERSPECTIVE,
		VIEW,

		MATRICES_COUNT
	};
public:
	OGL_Scene() = default;
	OGL_Scene(const OGL_Scene&) = delete;
	OGL_Scene(OGL_Scene&&) = delete;
	~OGL_Scene();

	auto	CreateBuffers() -> void;
	auto	FreeOGLResources() -> void;

	auto	Render() -> void;

	auto	AddMesh(OGL_Mesh* _mesh) -> void { m_Meshes.push_back(_mesh); }
	auto	AddModel(OGL_Model& _model) -> void { m_Models.push_back(&_model); }

	auto	CenterCamera(Vec3 _min, Vec3 _max, float _FOV) -> void;

	auto	GetCameraTransform() -> Transform& { return m_Camera; }
	auto	SetPerspectiveMatrix(GLfloat* _matrix) -> void;
	auto	SetViewMatrix(GLfloat* _matrix) -> void;

	auto	operator = (const OGL_Scene&)->OGL_Scene& = delete;
	auto	operator = (OGL_Scene&&)->OGL_Scene& = delete;

private:
	Transform							m_Camera;
	std::vector<OGL_Model*>				m_Models;
	std::vector<OGL_Mesh*>				m_Meshes;
	std::vector<OGL_InstancedMesh*>		m_InstancedMeshes;

	GLuint			m_MatricesBuffer = 0;
	GLfloat			m_Matrices[MATRICES_COUNT][16];

};


#endif /*__OGL_SCENE_HPP__*/