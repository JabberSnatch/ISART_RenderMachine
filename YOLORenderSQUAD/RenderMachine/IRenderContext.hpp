#ifndef __IRENDER_CONTEXT_HPP__
#define __IRENDER_CONTEXT_HPP__


class IRenderContext
{
public:
	enum E_TYPE { OPENGL, D3D11, COUNT };

	IRenderContext() = default;
	IRenderContext(const IRenderContext&) = delete;
	IRenderContext(IRenderContext&&) = delete;
	virtual ~IRenderContext() = default;

	virtual void	ClearBuffer() = 0;
	virtual void	SwapBuffers() = 0;
	virtual void	Shutdown() = 0;

	virtual E_TYPE	Type() = 0;

	auto	operator = (const IRenderContext&) -> IRenderContext& = delete;
	auto	operator = (IRenderContext&&) -> IRenderContext& = delete;
};


#endif /*__IRENDER_CONTEXT_HPP__*/