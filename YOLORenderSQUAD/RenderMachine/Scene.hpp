#ifndef __SCENE_HPP__
#define __SCENE_HPP__

#include <map>

#include "UUID.hpp"
#include "Node.hpp"

class Camera; // :public Component
class IRenderObject; // :public Component
class IDynamicObject; // :public Component
class Light; // :public Component


typedef std::map<utility::UUID, IRenderObject*>		RenderObjectMap_t;
typedef std::map<utility::UUID, IDynamicObject*>	DynamicObjectMap_t;
typedef std::map<utility::UUID, Light*>				LightMap_t;

class Scene
{
public:
	Scene();
	Scene(const Scene&) = delete;
	Scene(Scene&&) = delete;
	~Scene() = default;

	Node*		Root() { return m_Root; }
	Camera*		MainCamera() const { return m_MainCamera; }

	void		AddRenderObject(IRenderObject* _object);
	void		AddDynamicObject(IDynamicObject* _object);
	void		AddLight(Light* _light);

	const RenderObjectMap_t&	RenderObjectsMap() const { return m_RenderObjectsMap; }
	const DynamicObjectMap_t&	DynamicObjectsMap() const { return m_DynamicObjectsMap; }
	const LightMap_t&			LightsMap() const { return m_LightsMap; }

	auto	operator = (const Scene&) -> Scene& = delete;
	auto	operator = (Scene&&) -> Scene& = delete;

private:
	Node*		m_Root = nullptr;
	Camera*		m_MainCamera;

	RenderObjectMap_t		m_RenderObjectsMap;
	DynamicObjectMap_t		m_DynamicObjectsMap;
	LightMap_t				m_LightsMap;
};


#endif /*__SCENE_HPP__*/