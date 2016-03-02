#include "OGL_RenderContext.hpp"

#include <Windows.h>
#include <iostream>
#include <glew\include\GL\glew.h>
#include <glew\include\GL\wglew.h>

void
OGL_RenderContext::Initialize(HWND _window)
{
	m_Window = _window;
	m_DeviceContext = GetDC(m_Window);

	PIXELFORMATDESCRIPTOR PixelFormat = { 0 };
	PixelFormat.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	PixelFormat.nVersion = 1;
	PixelFormat.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
	PixelFormat.iPixelType = PFD_TYPE_RGBA;
	PixelFormat.cColorBits = 32;
	PixelFormat.cDepthBits = 24;
	PixelFormat.cStencilBits = 8;
	PixelFormat.iLayerType = PFD_MAIN_PLANE;

	int nPixelFormat = ChoosePixelFormat(m_DeviceContext, &PixelFormat);
	SetPixelFormat(m_DeviceContext, nPixelFormat, &PixelFormat);

	m_glRenderContext = wglCreateContext(m_DeviceContext);
	wglMakeCurrent(m_DeviceContext, m_glRenderContext);

	if (glewInit() != GLEW_OK)
		printf("glew failed to initialize.\n");

	int glAttribs[] = {
		WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
		WGL_CONTEXT_MINOR_VERSION_ARB, 5,
		WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
		0
	};

	if (wglewIsSupported("WGL_ARB_create_context") == 1)
	{
		HGLRC temp = m_glRenderContext;
		m_glRenderContext = wglCreateContextAttribsARB(m_DeviceContext, 0, glAttribs);
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(temp);
		wglMakeCurrent(m_DeviceContext, m_glRenderContext);
	}

	std::cout << "Version OpenGL : " << glGetString(GL_VERSION) << std::endl;
	std::cout << "Fabriquant : " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "Pilote : " << glGetString(GL_RENDERER) << std::endl;

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(.5f, .5f, .5f, 1.f);
}


void
OGL_RenderContext::ClearBuffer()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);
}


void
OGL_RenderContext::SwapBuffers()
{
	::SwapBuffers(m_DeviceContext);
}


void
OGL_RenderContext::Shutdown()
{
	wglMakeCurrent(m_DeviceContext, NULL);
	wglDeleteContext(m_glRenderContext);
}