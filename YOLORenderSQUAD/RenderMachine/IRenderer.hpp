#ifndef __IRENDERER_HPP__
#define __IRENDERER_HPP__

#include "IRenderContext.hpp"


class IRenderer
{
public:
	IRenderer() = default;
	IRenderer(const IRenderer&) = delete;
	IRenderer(IRenderer&&) = delete;
	virtual ~IRenderer() = default;

	virtual void	Initialize(IRenderContext* _context) = 0;
	virtual void	Render() = 0;
	virtual void	Shutdown() = 0;

	auto	operator = (const IRenderer&) -> IRenderer& = delete;
	auto	operator = (IRenderer&&) -> IRenderer& = delete;

private:

};


#endif /*__IRENDERER_HPP__*/