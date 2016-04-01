#ifndef __NODE_HPP__
#define __NODE_HPP__

#include <map>

#include "Matrix.hpp"
#include "UUID.hpp"
#include "Transform.hpp"


class Node
{
	friend class NodeIncubator;
public:
	Node(const Node&) = delete;
	Node(Node&&) = delete;
	~Node() = default;

	Node*		Parent() { return m_Parent; }

	void		SetParent(Node* _parent);

	void		AddChild(Node* _child);
	Node*		CreateChild();
	bool		RemoveChild(Node* _child);

	Transform&	LocalTransform() { return m_LocalTransform; }
	Transform	WorldTransform() const;

	Matrix		LocalMatrix() const;
	Matrix		WorldMatrix() const;
	Matrix		InverseWorldMatrix() const;

	auto	operator = (const Node&) -> Node& = delete;
	auto	operator = (Node&&) -> Node& = delete;

private:
	Node() = default;

	utility::UUID			m_ID;
	Transform				m_LocalTransform;

	Node*					m_Parent = nullptr;
	std::map<utility::UUID, Node*>	m_Children;
};


#endif /*__NODE_HPP__*/
