#ifndef __DEVICE_HPP__
#define __DEVICE_HPP__

#include "IRenderer.hpp"
#include "IRenderContext.hpp"
#include "NodeIncubator.hpp"
#include "ComponentIncubator.hpp"
#include "Input.hpp"

#include "Scene.hpp"

#include "OGL_Scene.hpp"

class Device
{
public:
	static Device*	Instance() 
	{
		if (!g_Instance)
			g_Instance = new Device();
		return g_Instance;
	}
	static void		Kill()
	{
		if (g_Instance)
			delete g_Instance;
	}

	Device(const Device&) = delete;
	Device(Device&&) = delete;
	~Device() = default;

	void	Initialize(int _width = 1280, int _height = 720);
	void	Shutdown();

	void	Update(double _deltaTime);
	void	Render();
	void	SwapBuffers() { m_RenderContext->SwapBuffers(); }

	void	SetDimensions(int _width, int _height);
	void	SetRenderer(IRenderer* _renderer) { m_Renderer = _renderer; }
	void	SetRenderContext(IRenderContext* _context) { m_RenderContext = _context; }

	Scene*				CurrentScene() { return m_CurrentScene; }
	ComponentIncubator*	getComponentIncubator() { return &m_ComponentIncubator; }
	NodeIncubator*		getNodeIncubator() { return &m_NodeIncubator; }
	InputManager*		getInputManager() { return &m_InputManager; }

	auto	operator = (const Device&) -> Device& = delete;
	auto	operator = (Device&&) -> Device& = delete;

private:
	bool	m_Init = false;

	int		m_Width;
	int		m_Height;

	IRenderer*			m_Renderer;
	IRenderContext*		m_RenderContext;
	Scene*				m_CurrentScene;

	ComponentIncubator	m_ComponentIncubator;
	NodeIncubator		m_NodeIncubator;
	InputManager		m_InputManager;

	// This will change at some point
	OGL_Scene		m_OGL_Scene;
	OGL_Shader		m_Shader;
	OGL_Model		m_Model;


	Device() = default;
	static Device*		g_Instance;
};

#define DEVICE				Device::Instance()
#define COMPONENTINCUBATOR	Device::Instance()->getComponentIncubator()
#define NODEINCUBATOR		Device::Instance()->getNodeIncubator()
#define INPUT				Device::Instance()->getInputManager()
#define ROOTNODE			Device::Instance()->CurrentScene()->Root()
#define MAINCAMERANODE		Device::Instance()->CurrentScene()->MainCamera()->getNode()

#endif /*__DEVICE_HPP__*/