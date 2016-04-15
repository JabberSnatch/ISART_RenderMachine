#ifndef __OGL_DEFERRED_RENDERER_HPP__
#define __OGL_DEFERRED_RENDERER_HPP__

#include "OGL_Renderer.hpp"
#include "OGL_Shader.hpp"
#include "OGL_Framebuffer.hpp"

#include "glew/include/GL/glew.h"


class OGL_DeferredRenderer
	:public OGL_Renderer
{
public:
	struct RenderTargetParam
	{
		std::string		Identifier;
		GLenum			InternalFormat;
		GLenum			AttachmentPoint;
	};

	OGL_DeferredRenderer(int _width, int _height);
	OGL_DeferredRenderer() = delete;
	OGL_DeferredRenderer(const OGL_DeferredRenderer&) = delete;
	OGL_DeferredRenderer(OGL_DeferredRenderer&&) = delete;
	virtual ~OGL_DeferredRenderer() = default;

	virtual void	Initialize() override;
	virtual void	Render(const Scene* _scene) override;
	virtual void	Shutdown() override;

	virtual void	Resize(int _width, int _height) override;
	
	auto	operator = (const OGL_DeferredRenderer&) -> OGL_DeferredRenderer& = delete;
	auto	operator = (OGL_DeferredRenderer&&) -> OGL_DeferredRenderer& = delete;

private:
	void	LightingPass(const LightMap_t& _lights, const Transform& _cam, std::initializer_list<std::string> _sourceRenderTargets);
	void	DebugDrawBuffer(int _target);

	// TODO: Probably bring these up to IRenderer class.
	int		m_Width;
	int		m_Height;

	OGL_Shader	m_GeometryPass;
	OGL_Shader	m_LightingPass;
	OGL_Shader	m_SolidBackgroundPass;
	OGL_Shader	m_QuadShader;

	OGL_Framebuffer	m_Framebuffer;

	// NOTE: Defined in .cpp
	static const std::vector<RenderTargetParam> AvailableTargets;
};


#endif /*__OGL_DEFERRED_RENDERER_HPP__*/