#ifndef __IDYNAMIC_OBJECT_HPP__
#define __IDYNAMIC_OBJECT_HPP__

#include "Component.hpp"


class IDynamicObject
	:public Component
{
public:
	IDynamicObject() = default;
	IDynamicObject(const IDynamicObject&) = delete;
	IDynamicObject(IDynamicObject&&) = delete;
	virtual ~IDynamicObject() = default;

	virtual void	Attach(Node* _node);
	virtual void	Update(double _deltaTime) = 0;

	auto	operator = (const IDynamicObject&) -> IDynamicObject& = delete;
	auto	operator = (IDynamicObject&&) -> IDynamicObject& = delete;

};


#endif /*__IDYNAMIC_OBJECT_HPP__*/