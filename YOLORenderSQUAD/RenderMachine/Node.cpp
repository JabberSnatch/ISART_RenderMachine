#include "Node.hpp"

#include <list>


Matrix
Node::GetLocalMatrix() const
{
	return m_LocalTransform.GetMatrix();
}


Matrix
Node::GetWorldMatrix() const
{
	//std::list<Node*> 
	return Matrix();
}