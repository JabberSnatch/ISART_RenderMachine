#include "OGL_RenderContext.hpp"

#include <QWindow>
#include <QOpenGLContext>
#include <iostream>

void
OGL_RenderContext::Initialize(QWindow* _win)
{
	QSurfaceFormat glFormat;
	glFormat.setMajorVersion(4);
	glFormat.setMinorVersion(3);
	glFormat.setProfile(QSurfaceFormat::CompatibilityProfile);
	glFormat.setRenderableType(QSurfaceFormat::OpenGL);
	glFormat.setSwapBehavior(QSurfaceFormat::DoubleBuffer);

	_win->setFormat(glFormat);
	_win->setSurfaceType(QWindow::SurfaceType::OpenGLSurface);
	_win->create();
	m_Window = _win;

	m_QtContext = new QOpenGLContext();
	m_QtContext->setFormat(glFormat);
	m_QtContext->create();
	m_QtContext->makeCurrent(_win);

	if (!m_QtContext->isValid())
		printf("GL context was not initialized properly\n");

	GLenum err = glewInit();
	if (err != GLEW_OK)
		printf("glew failed to initialize : %s\n", glewGetErrorString(err));

	std::cout << "Version OpenGL : " << glGetString(GL_VERSION) << std::endl;
	std::cout << "Fabriquant : " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "Pilote : " << glGetString(GL_RENDERER) << std::endl;

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(.5f, .5f, .5f, 1.f);
}


void
OGL_RenderContext::ClearBuffer()
{
	m_QtContext->makeCurrent(m_Window);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


void
OGL_RenderContext::SwapBuffers()
{
	m_QtContext->swapBuffers(m_Window);
	m_QtContext->doneCurrent();
}


void
OGL_RenderContext::Shutdown()
{
	delete m_QtContext;
	m_Window->destroy();
}

