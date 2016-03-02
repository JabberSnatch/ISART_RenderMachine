#ifndef __COMPONENT_INCUBATOR_HPP__
#define __COMPONENT_INCUBATOR_HPP__

#include <map>
#include <list>
#include "UUID.hpp"


class Component;
class Node;

class ComponentIncubator
{
public:
	ComponentIncubator() = default;
	ComponentIncubator(const ComponentIncubator&) = delete;
	ComponentIncubator(ComponentIncubator&&) = delete;
	~ComponentIncubator();

	Component*	Create();
	bool		Delete(Component* _component);

	void		GarbageCollection();

	auto	operator = (const ComponentIncubator&) -> ComponentIncubator& = delete;
	auto	operator = (ComponentIncubator&&) -> ComponentIncubator& = delete;

private:
	struct ComponentMeta
	{
		Component*		m_Component = nullptr;
		bool			m_Delete = false;
	};
	typedef std::map<UUID, ComponentMeta> ComponentMap_t;

	ComponentMap_t		m_ComponentMap;
};


#endif /*__COMPONENT_INCUBATOR_HPP__*/