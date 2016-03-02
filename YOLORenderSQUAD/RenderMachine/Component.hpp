#ifndef __COMPONENT_HPP__
#define __COMPONENT_HPP__

#include "UUID.hpp"

class Node;

class Component
{
	friend class ComponentIncubator;
public:
	Component(const Component&) = delete;
	Component(Component&&) = delete;
	virtual ~Component() = default;

	auto	operator = (const Component&) -> Component& = delete;
	auto	operator = (Component&&) -> Component& = delete;

private:
	Component() = default;

	UUID	m_ID;
	
	Node*	m_Node;

};


#endif /*__COMPONENT_HPP__*/