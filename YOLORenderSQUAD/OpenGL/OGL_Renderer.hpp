#ifndef __OGL_RENDERER_HPP__
#define __OGL_RENDERER_HPP__

#include "IRenderer.hpp"

#include "glew\include\GL\glew.h"


#define MAX_LIGHT_COUNT 50
#define DIRLIGHT_SIZE (4 * 4 * sizeof(GLfloat))
#define POINTLIGHT_SIZE (4 * 4 * sizeof(GLfloat) + 4 * sizeof(GLfloat))
#define SPOTLIGHT_SIZE (5 * 4 * sizeof(GLfloat) + 4 * sizeof(GLfloat))

#define ARRAY_OFFSET 4 * sizeof(GLuint)

#define LIGHTS_BUFFER_BASE 1
#define MATRICES_BUFFER_BASE 42


class OGL_Renderer
	:public IRenderer
{
public:
	OGL_Renderer() = default;
	OGL_Renderer(const OGL_Renderer&) = delete;
	OGL_Renderer(OGL_Renderer&&) = delete;
	virtual ~OGL_Renderer() = default;

	virtual void	Initialize();
	virtual void	Render(const Scene* _scene);
	virtual void	ImGui_RenderDrawLists(ImDrawData* _data);
	virtual void	Shutdown();

	auto	operator = (const OGL_Renderer&) -> OGL_Renderer& = delete;
	auto	operator = (OGL_Renderer&&) -> OGL_Renderer& = delete;

protected:
	void	BindLightIntoBuffer(const Light& _light, unsigned int _lightIndex) const;
	GLint	ComputeLightOffset(const Light& _light, unsigned int _lightIndex) const;

	GLuint	m_LightsBuffer = 0;
	GLuint	m_MatricesBuffer = 0;

};



#endif /*__OGL_RENDERER_HPP__*/