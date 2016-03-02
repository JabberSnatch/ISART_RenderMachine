#ifndef __OGL_RENDER_CONTEXT_HPP__
#define __OGL_RENDER_CONTEXT_HPP__

#include <Windows.h>
#include "IRenderContext.hpp"


class OGL_RenderContext
	:public IRenderContext
{
public:
	OGL_RenderContext() = default;
	OGL_RenderContext(const OGL_RenderContext&) = delete;
	OGL_RenderContext(OGL_RenderContext&&) = delete;
	virtual ~OGL_RenderContext() = default;

	void			Initialize(HWND _window);
	virtual void	ClearBuffer();
	virtual void	SwapBuffers();
	virtual void	Shutdown();
	virtual E_TYPE	Type() { return OPENGL; }

	auto	operator = (const OGL_RenderContext&) -> OGL_RenderContext& = delete;
	auto	operator = (OGL_RenderContext&&) -> OGL_RenderContext& = delete;

private:
	HWND	m_Window = nullptr;
	HDC		m_DeviceContext = nullptr;
	HGLRC	m_glRenderContext = nullptr;

};


#endif /*__OGL_RENDER_CONTEXT_HPP__*/