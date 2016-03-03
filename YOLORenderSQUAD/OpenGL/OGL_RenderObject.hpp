#ifndef __OGL_RENDER_OBJECT_HPP__
#define __OGL_RENDER_OBJECT_HPP__

#include "IRenderObject.hpp"
#include "Node.hpp"

#include "OGL_Mesh.hpp"

class OGL_RenderObject
	:public IRenderObject
{
public:
	OGL_RenderObject() = default;
	OGL_RenderObject(const OGL_RenderObject&) = delete;
	OGL_RenderObject(OGL_RenderObject&&) = delete;
	virtual ~OGL_RenderObject() = default;

	virtual void	Render();

	void	AddMesh(const MeshData& _source, OGL_Shader* _shader);
	void	AddMultiMesh(const MultiMeshData& _source, OGL_Shader* _shader);

	Vec3		Min() { return m_Min * getNode()->WorldTransform().Scale; }
	Vec3		Max() { return m_Max * getNode()->WorldTransform().Scale; }
	
	OGL_Mesh&				Mesh(int i) { return m_Meshes[i]; }
	std::vector<OGL_Mesh>&	Meshes() { return m_Meshes; }

	auto	operator = (const OGL_RenderObject&) -> OGL_RenderObject& = delete;
	auto	operator = (OGL_RenderObject&&) -> OGL_RenderObject& = delete;

private:
	Vec3					m_Min = Vec3::Zero();
	Vec3					m_Max = Vec3::Zero();

	std::vector<OGL_Mesh>	m_Meshes;

};


#endif /*__OGL_RENDER_OBJECT_HPP__*/