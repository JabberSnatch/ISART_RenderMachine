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

	void		Shutdown();

	template <class T>
	T*			Create();
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
	typedef std::map<utility::UUID, ComponentMeta> ComponentMap_t;

	ComponentMap_t		m_ComponentMap;
};


template <class T> T*
ComponentIncubator::Create()
{
	T* result = new T();
	ComponentMeta meta; meta.m_Component = (Component*)result;
	m_ComponentMap.emplace(meta.m_Component->m_ID, meta);
	return result;
}


#endif /*__COMPONENT_INCUBATOR_HPP__*/