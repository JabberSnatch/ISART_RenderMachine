#include "ComponentIncubator.hpp"

#include "Component.hpp"

ComponentIncubator::~ComponentIncubator()
{
	Shutdown();
}


void
ComponentIncubator::Shutdown()
{
	for (ComponentMap_t::iterator ite = m_ComponentMap.begin(); ite != m_ComponentMap.end(); ++ite)
		delete ite->second.m_Component;

	m_ComponentMap.clear();
}


bool
ComponentIncubator::Delete(Component* _component)
{
	bool result = false;

	ComponentMap_t::iterator pair = m_ComponentMap.find(_component->m_ID);
	if (pair != m_ComponentMap.end())
	{
		pair->second.m_Delete = true;
		result = true;
	}

	return result;
}


void
ComponentIncubator::GarbageCollection()
{
	std::list<utility::UUID> toDelete;
	for (ComponentMap_t::iterator ite = m_ComponentMap.begin(); ite != m_ComponentMap.end(); ++ite)
	{
		if (ite->second.m_Delete)
			toDelete.push_back(ite->first);
	}

	for (std::list<utility::UUID>::iterator ite = toDelete.begin(); ite != toDelete.end(); ++ite)
	{
		delete m_ComponentMap[*ite].m_Component;
		m_ComponentMap.erase(*ite);
	}
}
