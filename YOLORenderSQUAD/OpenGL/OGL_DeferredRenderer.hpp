#ifndef __OGL_DEFERRED_RENDERER_HPP__
#define __OGL_DEFERRED_RENDERER_HPP__

#include "OGL_Renderer.hpp"

#include "glew/include/GL/glew.h"


class OGL_DeferredRenderer
	:public OGL_Renderer
{
public:
	enum RenderTargetMask
	{
		NONE = 0,
		POSITION_MASK = 0x1,
		NORMAL_MASK = 0x2,
		DIFFUSE_SPEC_MASK = 0x4
	};

	enum RenderTarget
	{
		POSITION = 0,
		NORMAL,
		DIFFUSE_SPEC,
		RENDER_TARGET_COUNT
	};

	struct RenderTargetDesc
	{
		GLuint	LayoutPosition;
		GLuint	InternalFormat;
		GLuint	Format;
		GLuint	Type;
	};

	OGL_DeferredRenderer() = default;
	OGL_DeferredRenderer(const OGL_DeferredRenderer&) = delete;
	OGL_DeferredRenderer(OGL_DeferredRenderer&&) = delete;
	virtual ~OGL_DeferredRenderer() = default;

	virtual void	Initialize() override;
	virtual void	Render(const Scene* _scene) override;
	virtual void	Shutdown() override;

	void	Resize(int _width, int _height) {};
	
	// TODO: Implement these if we find a use to making the DeferredRenderer tweakable
	//void	EnableRenderTargets(unsigned int _mask);
	//void	AllocateBuffers();

	auto	operator = (const OGL_DeferredRenderer&) -> OGL_DeferredRenderer& = delete;
	auto	operator = (OGL_DeferredRenderer&&) -> OGL_DeferredRenderer& = delete;

private:
	void	DrawScreenQuad();

	// TODO: Probably bring these up to IRenderer class.
	int		m_Width;
	int		m_Height;

	GLuint	m_FrameBuffer = 0;
	GLuint	m_RenderTextures[RENDER_TARGET_COUNT];


	static const RenderTargetDesc AvailableTargets[RENDER_TARGET_COUNT];
};


const OGL_DeferredRenderer::RenderTargetDesc
OGL_DeferredRenderer::AvailableTargets[RENDER_TARGET_COUNT] =
{
	{POSITION, GL_RGB32F, GL_RGB, GL_FLOAT},
	{NORMAL, GL_RGB32F, GL_RGB, GL_FLOAT},
	{DIFFUSE_SPEC, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE}
};


#endif /*__OGL_DEFERRED_RENDERER_HPP__*/