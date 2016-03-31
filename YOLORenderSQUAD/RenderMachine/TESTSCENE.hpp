#ifndef __TEST_SCENE_HPP__
#define __TEST_SCENE_HPP__

#include <fstream>
#include "OGL_Shader.hpp"
#include "OGL_RenderObject.hpp"
#include "Node.hpp"
#include "Camera.hpp"
#include "Light.hpp"
#include "RotateAround.hpp"
OGL_Shader g_Shader;
void
INIT_TEST_SCENE()
{
	g_Shader.LoadShaderAndCompile("../Resources/SHADERS/IlluminationShader.vs", GL_VERTEX_SHADER);
	g_Shader.LoadShaderAndCompile("../Resources/SHADERS/IlluminationShader.fs", GL_FRAGMENT_SHADER);
	g_Shader.LinkShaders();


	ObjParser parser;
	MultiMeshData data;

	//std::string name = "_zero_model/zero";
	std::string name = "../Resources/MODELS/_ciri_model/ciri";
	//std::string name = "_lightning_model/lightning_obj";
	//std::string name = "sphere";

	if (!std::fstream(name + ".mys").good())
	{
		parser.ParseFile(name + ".obj");
//#ifndef WIN64
		data = parser.GenerateMeshData();
		data.Serialize(name + ".mys");
	}
	else
	{
//#else
		data.Deserialize(name + ".mys");
	}
//#endif


	Node* modelNode = ROOTNODE->CreateChild();
	modelNode->LocalTransform().Position = Vec3(0.f, 8.f, 1.f);
	RotateAround* controller = COMPONENTINCUBATOR->Create<RotateAround>();
	controller->Attach(modelNode);
	//controller->Attach(MAINCAMERANODE);

	Node* offsetNode = modelNode->CreateChild();
	offsetNode->LocalTransform().Position = Vec3(0.f, -8.f, -1.f);
	OGL_RenderObject* model = COMPONENTINCUBATOR->Create<OGL_RenderObject>();
	model->AddMultiMesh(data, &g_Shader);
	model->Attach(offsetNode);


	MAINCAMERANODE->LocalTransform().Position = Vec3(0.f, 8.f, 15.f);
	MAINCAMERANODE->LocalTransform().Rotation = Quaternion(180.f, Vec3::Up());

	modelNode->LocalTransform().Scale = Vec3(0.02f);
	//m_OGL_Scene.CenterCamera(m_Model.GetMin(), m_Model.GetMax(), 60.f);


	Node* lightNode = NODEINCUBATOR->Create();
	Light* light0 = COMPONENTINCUBATOR->Create<Light>();
	light0->Attach(lightNode);
	light0->m_Type = Light::DIRECTIONAL;
	light0->m_Ia = Vec3(.25f, .2f, .15f);
	light0->m_Id = Vec3(0.8f, 0.75f, 0.75f);
	light0->m_Is = Vec3(.8f, .8f, .7f);
	light0->m_Direction = Vec3(0.f, 0.f, 1.f);

	lightNode = NODEINCUBATOR->Create();
	Light* light1 = COMPONENTINCUBATOR->Create<Light>();
	light1->Attach(lightNode);
	light1->m_Type = Light::POINT;
	light1->m_Ia = Vec3(0.f, 0.f, 0.f);
	light1->m_Id = Vec3(0.f, 0.f, 1.f);
	light1->m_Is = Vec3(0.f, 0.f, 1.f);
	light1->m_Position = Vec3(5.f, 5.f, 0.f);

	lightNode = NODEINCUBATOR->Create();
	Light* light2 = COMPONENTINCUBATOR->Create<Light>();
	light2->Attach(lightNode);
	light2->m_Type = Light::DIRECTIONAL;
	light2->m_Ia = Vec3(0.f, 0.f, 0.f);
	light2->m_Id = Vec3(1.f, 0.f, 0.f);
	light2->m_Is = Vec3(1.f, 0.f, 0.f);
	light2->m_Direction = Vec3(0.f, 1.f, -1.f);

	lightNode = NODEINCUBATOR->Create();
	Light* light3 = COMPONENTINCUBATOR->Create<Light>();
	light3->Attach(lightNode);
	light3->m_Type = Light::SPOT;
	light3->m_Ia = Vec3(0.f, 0.f, 0.f);
	light3->m_Id = Vec3(0.f, 1.f, 0.f);
	light3->m_Is = Vec3(0.f, 1.f, 0.f);
	light3->m_Direction = Vec3(0.f, -0.8f, 1.f);
	light3->m_Position = Vec3(0.f, 20.f, -5.f);
	light3->m_Cutoff = 25.f;
}

#endif /*__TEST_SCENE_HPP__*/