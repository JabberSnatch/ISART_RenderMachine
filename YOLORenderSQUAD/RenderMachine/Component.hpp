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

	virtual void Attach(Node* _node) { m_Node = _node; }

	utility::UUID	UniqueID() { return m_ID; }
	Node*	getNode() { return m_Node; }
	Node*	c_getNode() const { return m_Node; }

	auto	operator = (const Component&) -> Component& = delete;
	auto	operator = (Component&&) -> Component& = delete;

protected:
	Component() = default;

private:
	utility::UUID	m_ID;
	
	Node*			m_Node;

};


#endif /*__COMPONENT_HPP__*/