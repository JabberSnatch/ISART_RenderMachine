#ifndef __OGL_SKYBOX_HPP__
#define __OGL_SKYBOX_HPP__

#include "ISkybox.hpp"
#include "OGL_Mesh.hpp"


class OGL_Skybox
	: public ISkybox
{
public:
	OGL_Skybox() = default;
	OGL_Skybox(const OGL_Skybox&) = delete;
	OGL_Skybox(OGL_Skybox&&) = delete;
	virtual ~OGL_Skybox();

	void			AllocateResources();
	virtual void	Render();

	GLuint			Cubemap() { return m_Cubemap; }
	void			SetBoxMesh(const MeshData& _source, OGL_Shader* _shader);

	auto	operator = (const OGL_Skybox&) -> OGL_Skybox& = delete;
	auto	operator = (OGL_Skybox&&) -> OGL_Skybox& = delete;

private:
	OGL_Mesh*	m_Box = nullptr;
	GLuint		m_Cubemap = 0;

};


#endif /*__OGL_SKYBOX_HPP__*/