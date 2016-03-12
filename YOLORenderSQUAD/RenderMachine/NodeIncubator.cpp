#include "NodeIncubator.hpp"

#include <list>


NodeIncubator::~NodeIncubator()
{
	Shutdown();
}


void
NodeIncubator::Shutdown()
{
	for (NodeMap_t::iterator ite = m_NodeMap.begin(); ite != m_NodeMap.end(); ++ite)
		delete ite->second.m_Node;

	m_NodeMap.clear();
}


Node*
NodeIncubator::Create()
{
	Node* result = new Node();
	NodeMeta meta; meta.m_Node = result;
	m_NodeMap.emplace(result->m_ID, meta);
	return result;
}


bool
NodeIncubator::Delete(Node* _node)
{
	bool result = false;

	NodeMap_t::iterator pair = m_NodeMap.find(_node->m_ID);
	if (pair != m_NodeMap.end())
	{
		pair->second.m_Delete = true;
		result = true;
	}

	return result;
}


void
NodeIncubator::GarbageCollection()
{
	std::list<utility::UUID> toDelete;
	for (NodeMap_t::iterator ite = m_NodeMap.begin(); ite != m_NodeMap.end(); ++ite)
	{
		if (ite->second.m_Delete)
			toDelete.push_back(ite->first);
	}

	for (std::list<utility::UUID>::iterator ite = toDelete.begin(); ite != toDelete.end(); ++ite)
	{
		delete m_NodeMap[*ite].m_Node;
		m_NodeMap.erase(*ite);
	}
}

