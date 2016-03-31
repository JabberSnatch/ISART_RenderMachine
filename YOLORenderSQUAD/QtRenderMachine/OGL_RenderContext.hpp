#ifndef __OGL_RENDER_CONTEXT_HPP__
#define __OGL_RENDER_CONTEXT_HPP__

#include "IRenderContext.hpp"
#include <glew\include\GL\glew.h>

class QWindow;
class QOpenGLContext;


class OGL_RenderContext
	: public IRenderContext
{
public:
	OGL_RenderContext() = default;
	OGL_RenderContext(const OGL_RenderContext&) = delete;
	OGL_RenderContext(OGL_RenderContext&&) = delete;
	~OGL_RenderContext() = default;

	void			Initialize(QWindow* _win);
	virtual void	ClearBuffer();
	virtual void	SwapBuffers();
	virtual void	Shutdown();

	virtual E_TYPE	Type() { return E_TYPE::OPENGL; }

	auto	operator = (const OGL_RenderContext&) -> OGL_RenderContext& = delete;
	auto	operator = (OGL_RenderContext&&) -> OGL_RenderContext& = delete;

private:
	QWindow*			m_Window = nullptr;
	QOpenGLContext*		m_QtContext = nullptr;

};


#endif /*__OGL_RENDER_CONTEXT_HPP__*/