#include "Device.hpp"

#include "IDynamicObject.hpp"
#include "Camera.hpp"


Device* Device::g_Instance = nullptr;


void
Device::Initialize(int _width, int _height)
{
	m_Width = _width;
	m_Height = _height;

	m_CurrentScene = new Scene();
	Camera* camera = m_CurrentScene->MainCamera();
	camera->Initialize({ 0, 0, _width, _height });
	camera->ComputePerspective();

	m_Init = true;
}


void
Device::Shutdown()
{
	if (m_RenderContext)
	{
		m_RenderContext->Shutdown();
		delete m_RenderContext;
	}

	delete m_CurrentScene;

	m_ComponentIncubator.Shutdown();
	m_NodeIncubator.Shutdown();
}


void
Device::Update(double _deltaTime)
{
	if (!m_Init) return;

	const DynamicObjectMap_t& objectsMap = m_CurrentScene->DynamicObjectsMap();
	for (DynamicObjectMap_t::const_iterator ite = objectsMap.cbegin(); ite != objectsMap.cend(); ++ite)
		(*ite).second->Update(_deltaTime);

	m_InputManager.SwapSnapshots();
}


void
Device::Render()
{
	if (!m_Init) return;

	m_Renderer->Render(m_CurrentScene);
}


void
Device::SetDimensions(int _width, int _height)
{
	if (_width != m_Width || _height != m_Height)
	{
		m_Width = _width;
		m_Height = _height;
		if (m_CurrentScene)
			m_CurrentScene->MainCamera()->SetViewport({0, 0, _width, _height});
		if (m_Renderer)
			m_Renderer->Resize(_width, _height);
	}
}


