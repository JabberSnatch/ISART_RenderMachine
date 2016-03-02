#ifndef __NODE_HPP__
#define __NODE_HPP__

#include <map>

#include "Matrix.hpp"
#include "UUID.hpp"
#include "Transform.hpp"


class Node
{
public:
	Node(const Node&) = delete;
	Node(Node&&) = delete;
	~Node() = default;

	Matrix	GetLocalMatrix() const;
	Matrix	GetWorldMatrix() const;

	auto	operator = (const Node&) -> Node& = delete;
	auto	operator = (Node&&) -> Node& = delete;

private:
	Node() = default;

	UUID					m_ID;
	Transform				m_LocalTransform;

	Node*					m_Parent = nullptr;
	std::map<UUID, Node*>	m_Children;
};


#endif /*__NODE_HPP__*/
