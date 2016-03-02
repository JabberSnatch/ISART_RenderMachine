#ifndef __COMPONENT_HPP__
#define __COMPONENT_HPP__


class Node;

class Component
{
public:
	Component() = default;
	Component(const Component&) = delete;
	Component(Component&&) = delete;
	virtual ~Component() = default;

	auto	operator = (const Component&) -> Component& = delete;
	auto	operator = (Component&&) -> Component& = delete;

private:
	Node*	m_Node;

};


#endif /*__COMPONENT_HPP__*/