#ifndef __IRENDER_OBJECT_HPP__
#define __IRENDER_OBJECT_HPP__

#include "Component.hpp"


class IRenderObject
	:public Component
{
public:
	IRenderObject() = default;
	IRenderObject(const IRenderObject&) = delete;
	IRenderObject(IRenderObject&&) = delete;
	virtual ~IRenderObject() = default;

	virtual void	Attach(Node* _node);
	virtual void	Render() = 0;

	auto	operator = (const IRenderObject&) -> IRenderObject& = delete;
	auto	operator = (IRenderObject&&) -> IRenderObject& = delete;

};


#endif /*__IRENDER_OBJECT_HPP__*/