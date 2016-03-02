#ifndef __NODE_INCUBATOR_HPP__
#define __NODE_INCUBATOR_HPP__

#include <map>

#include "Node.hpp"


class NodeIncubator
{
public:
	NodeIncubator() = default;
	NodeIncubator(const NodeIncubator&) = delete;
	NodeIncubator(NodeIncubator&&) = delete;
	~NodeIncubator();

	Node*	Create();
	bool	Delete(Node* _node);

	void	GarbageCollection();

	auto	operator = (const NodeIncubator&) -> NodeIncubator& = delete;
	auto	operator = (NodeIncubator&&) -> NodeIncubator& = delete;

private:
	struct NodeMeta
	{
		Node*		m_Node = nullptr;
		bool		m_Delete = false;
	};
	typedef std::map<UUID, NodeMeta> NodeMap_t;

	NodeMap_t		m_NodeMap;


};


#endif /*__NODE_INCUBATOR_HPP__*/