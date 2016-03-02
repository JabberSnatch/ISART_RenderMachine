#ifndef __DEVICE_HPP__
#define __DEVICE_HPP__

#include "IRenderContext.hpp"
#include "NodeIncubator.hpp"
#include "ComponentIncubator.hpp"

#include "OGL_Scene.hpp"

class Device
{
public:
	Device() = default;
	Device(const Device&) = delete;
	Device(Device&&) = delete;
	~Device() = default;

	void	Initialize(int _width = 1280, int _height = 720);
	void	Shutdown();

	void	Update(double _deltaTime);
	void	Render();

	void	SetDimensions(int _width, int _height) { m_Width = _width; m_Height = _height; }
	void	SetRenderContext(IRenderContext* _context) { m_RenderContext = _context; }

	auto	operator = (const Device&) -> Device& = delete;
	auto	operator = (Device&&) -> Device& = delete;


	void	OGL_SETUP();

private:
	int		m_Width;
	int		m_Height;

	IRenderContext*	m_RenderContext;

	// This will change at some point
	OGL_Scene		m_Scene;
	OGL_Shader		m_Shader;
	OGL_Model		m_Model;

};


#endif /*__DEVICE_HPP__*/