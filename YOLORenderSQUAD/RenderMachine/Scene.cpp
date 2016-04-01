#include "Scene.hpp"

#include "Device.hpp"
#include "Camera.hpp"
#include "Light.hpp"
#include "IRenderObject.hpp"
#include "IDynamicObject.hpp"

Scene::Scene()
{
	m_Root = NODEINCUBATOR->Create();
	
	Node* TranslateCameraNode = m_Root->CreateChild();
	m_MainCamera = COMPONENTINCUBATOR->Create<Camera>();
	m_MainCamera->Attach(TranslateCameraNode);
}


void
Scene::AddRenderObject(IRenderObject* _object)
{
	if (m_RenderObjectsMap.find(_object->UniqueID()) == m_RenderObjectsMap.end())
		m_RenderObjectsMap.emplace(_object->UniqueID(), _object);
}


void
Scene::AddDynamicObject(IDynamicObject* _object)
{
	if (m_DynamicObjectsMap.find(_object->UniqueID()) == m_DynamicObjectsMap.end())
		m_DynamicObjectsMap.emplace(_object->UniqueID(), _object);
}


void
Scene::AddLight(Light* _light)
{
	if (m_LightsMap.find(_light->UniqueID()) == m_LightsMap.end())
		m_LightsMap.emplace(_light->UniqueID(), _light);
}
