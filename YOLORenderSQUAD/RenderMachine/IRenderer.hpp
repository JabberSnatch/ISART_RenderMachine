#ifndef __IRENDERER_HPP__
#define __IRENDERER_HPP__

#include "imgui.h"
#include "Scene.hpp"


class IRenderer
{
public:
	IRenderer() = default;
	IRenderer(const IRenderer&) = delete;
	IRenderer(IRenderer&&) = delete;
	virtual ~IRenderer() = default;

	virtual void	Initialize() = 0;
	virtual void	Render(const Scene* _scene) = 0;
	virtual void	Shutdown() = 0;

	virtual void	Resize(int _width, int _height) = 0;

	auto	operator = (const IRenderer&) -> IRenderer& = delete;
	auto	operator = (IRenderer&&) -> IRenderer& = delete;
};


#endif /*__IRENDERER_HPP__*/
