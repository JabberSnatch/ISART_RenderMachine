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
	camera->Initialize();
	camera->SetAspectRatio((float)_width / _height);
	camera->ComputePerspective();
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
}


#include "OGL_Object.h"
void
Device::Update(double _deltaTime)
{
	const DynamicObjectMap_t& objectsMap = m_CurrentScene->DynamicObjectsMap();
	for (DynamicObjectMap_t::const_iterator ite = objectsMap.cbegin(); ite != objectsMap.cend(); ++ite)
		(*ite).second->Update(_deltaTime);


	//static GLfloat projectionMatrix[16];
	//OGL_Object::ComputePerspectiveProjectionFOV(projectionMatrix, 60.f, (float)m_Width / m_Height, .1f, 1000.f);
	//m_OGL_Scene.SetPerspectiveMatrix(projectionMatrix);
	//m_OGL_Scene.CenterCamera(m_Model.GetMin(), m_Model.GetMax(), 60.f);
	//
	//m_Model.GetTransform().Rotation = Quaternion((float)_deltaTime * 79.f, Vec3::Up()) * m_Model.GetTransform().Rotation;
	//m_Model.GetTransform().Rotation = Quaternion((float)_deltaTime * 57.f, Vec3::Right()) * m_Model.GetTransform().Rotation;
	//m_Model.GetTransform().Rotation = Quaternion((float)_deltaTime * 83.f, Vec3::Forward()) * m_Model.GetTransform().Rotation;
	//m_OGL_Scene.GetCameraTransform().Rotation = Quaternion((float)_deltaTime * -15.f, Vec3::Up()) * m_OGL_Scene.GetCameraTransform().Rotation;
}


void
Device::Render()
{
	m_RenderContext->ClearBuffer();

	//m_OGL_Scene.Render();
	m_Renderer->Render(m_CurrentScene);

	m_RenderContext->SwapBuffers();
}


#define FREEGLUT_LIB_PRAGMAS 0
#include <Windows.h>
#include <glew/include/GL/wglew.h>

#if defined(_WIN64)
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32.lib")
#elif defined(_WIN32)
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32.lib")
#endif

#include <fstream>
#include "ObjParser.hpp"
#include "Matrix.hpp"

#include "OGL_Model.hpp"
#include "OGL_Mesh.hpp"
#include "OGL_Shader.hpp"
#include "OGL_Scene.hpp"
#include "OGL_TextureLoader.hpp"
void
Device::OGL_SETUP()
{
	m_Shader.LoadShaderAndCompile("../Resources/SHADERS/IlluminationShader.vs", GL_VERTEX_SHADER);
	m_Shader.LoadShaderAndCompile("../Resources/SHADERS/IlluminationShader.fs", GL_FRAGMENT_SHADER);
	m_Shader.LinkShaders();


	ObjParser parser;
	MultiMeshData data;

	//std::string name = "_zero_model/zero";
	std::string name = "../Resources/MODELS/_ciri_model/ciri";
	//std::string name = "_lightning_model/lightning_obj";
	//std::string name = "sphere";

	if (!std::fstream(name + ".mys").good())
	{
		parser.ParseFile(name + ".obj");
		data = parser.GenerateMeshData();
		data.Serialize(name + ".mys");
	}
	else
		data.Deserialize(name + ".mys");

	m_Model.AddMultiMesh(data, &m_Shader);
	m_OGL_Scene.AddModel(m_Model);

	static GLfloat projectionMatrix[16];
	OGL_Object::ComputePerspectiveProjectionFOV(projectionMatrix, 60.f, (float)m_Width / m_Height, .1f, 1000.f);

	m_OGL_Scene.CreateBuffers();
	m_OGL_Scene.SetPerspectiveMatrix(projectionMatrix);
	m_OGL_Scene.GetCameraTransform().Position = Vec3(0.f, 8.f, 15.f);

	m_Model.GetTransform().Scale = Vec3(0.02f);
	m_OGL_Scene.CenterCamera(m_Model.GetMin(), m_Model.GetMax(), 60.f);


	OGL_Light light0(OGL_Light::DIRECTIONAL);
	light0.m_Ia = Vec3(.25f, .2f, .15f);
	light0.m_Id = Vec3(0.8f, 0.75f, 0.75f);
	light0.m_Is = Vec3(.8f, .8f, .7f);
	light0.m_Direction = Vec3(0.f, 0.f, -1.f);

	OGL_Light light1(OGL_Light::POINT);
	light1.m_Ia = Vec3(0.f, 0.f, 0.f);
	light1.m_Id = Vec3(0.f, 0.f, 1.f);
	light1.m_Is = Vec3(0.f, 0.f, 1.f);
	light1.m_Position = Vec3(5.f, 5.f, 0.f);

	OGL_Light light2(OGL_Light::DIRECTIONAL);
	light2.m_Ia = Vec3(0.f, 0.f, 0.f);
	light2.m_Id = Vec3(1.f, 0.f, 0.f);
	light2.m_Is = Vec3(1.f, 0.f, 0.f);
	light2.m_Direction = Vec3(0.f, 1.f, 1.f);

	OGL_Light light3(OGL_Light::SPOT);
	light3.m_Ia = Vec3(0.f, 0.f, 0.f);
	light3.m_Id = Vec3(0.f, 1.f, 0.f);
	light3.m_Is = Vec3(0.f, 1.f, 0.f);
	light3.m_Direction = Vec3(0.f, -0.8f, 1.f);
	light3.m_Position = Vec3(0.f, 20.f, -5.f);
	light3.m_Cutoff = 25.f;

	m_OGL_Scene.AddLight(light0);
	m_OGL_Scene.AddLight(light1);
	m_OGL_Scene.AddLight(light2);
	m_OGL_Scene.AddLight(light3);
}

