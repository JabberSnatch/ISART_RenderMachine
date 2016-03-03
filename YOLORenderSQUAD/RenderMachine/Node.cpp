#include "Node.hpp"

#include <list>

#include "Device.hpp"

void
Node::SetParent(Node* _parent)
{
	m_Parent = _parent;
}


void
Node::AddChild(Node* _child)
{
	_child->m_Parent = this;
	m_Children.emplace(_child->m_ID, _child);
}


Node*
Node::CreateChild()
{
	Node* result = NODEINCUBATOR->Create();
	AddChild(result);
	return result;
}


bool
Node::RemoveChild(Node* _child)
{
	bool result = m_Children.find(_child->m_ID) != m_Children.end();

	if (result)
		m_Children.erase(_child->m_ID);

	return result;
}


Transform
Node::WorldTransform() const
{
	std::list<const Node*> ancestors;

	const Node* current = this;
	while (current->m_Parent)
	{
		ancestors.push_back(current->m_Parent);
		current = current->m_Parent;
	}

	Transform result = m_LocalTransform;

	for (std::list<const Node*>::const_iterator ite = ancestors.cbegin(); ite != ancestors.cend(); ++ite)
		result = (*ite)->m_LocalTransform * result;

	return result;
}


Matrix
Node::LocalMatrix() const
{
	return m_LocalTransform.GetMatrix();
}


Matrix
Node::WorldMatrix() const
{
	return WorldTransform().GetMatrix();
}


Matrix
Node::InverseWorldMatrix() const
{
	return WorldTransform().GetInverseMatrix();
}

