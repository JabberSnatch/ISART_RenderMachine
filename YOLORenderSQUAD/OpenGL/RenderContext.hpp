#ifndef __RENDER_CONTEXT_HPP__
#define __RENDER_CONTEXT_HPP__

#include "IRenderContext.hpp"


class RenderContext
	:public IRenderContext
{
public:
	RenderContext() = default;
	RenderContext(const RenderContext&) = delete;
	RenderContext(RenderContext&&) = delete;
	virtual ~RenderContext() = default;

	auto	operator = (const RenderContext&) -> RenderContext& = delete;
	auto	operator = (RenderContext&&) -> RenderContext& = delete;

private:

};


#endif /*__RENDER_CONTEXT_HPP__*/