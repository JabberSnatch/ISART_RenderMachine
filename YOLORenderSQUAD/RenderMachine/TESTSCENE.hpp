#ifndef __TEST_SCENE_HPP__
#define __TEST_SCENE_HPP__

#include <fstream>
#include "OGL_Shader.hpp"
#include "OGL_RenderObject.hpp"
#include "OGL_Skybox.hpp"
#include "Node.hpp"
#include "Camera.hpp"
#include "Light.hpp"
#include "RotateAround.hpp"
#include "CameraFlyAroundController.hpp"
OGL_Shader g_Shader;
void
INIT_TEST_SCENE()
{
	//std::string name = "../Resources/MODELS/_zero_model/zero";
	std::string name = "../Resources/MODELS/_ciri_model/ciri";
	//std::string name = "../Resources/MODELS/_door_whatever/door";
	//std::string name = "../Resources/MODELS/_lei_fang_circus/DOA5_Lei_Fang_Circus_";
	//std::string name = "_lightning_model/lightning_obj";
	//std::string name = "../Resources/MODELS/sphere";
	//std::string name = "../Resources/MODELS/Blade";
	//std::string name = "../Resources/MODELS/ciri";
	
	g_Shader.LoadShaderAndCompile("../Resources/SHADERS/IlluminationShader.vs", GL_VERTEX_SHADER);
	g_Shader.LoadShaderAndCompile("../Resources/SHADERS/IlluminationShader.fs", GL_FRAGMENT_SHADER);
	g_Shader.LinkShaders();

	//g_Shader.LoadShaderAndCompile("../Resources/SHADERS/shaderDuQ.vs", GL_VERTEX_SHADER);
	//g_Shader.LoadShaderAndCompile("../Resources/SHADERS/fsDuQ.fs", GL_FRAGMENT_SHADER);
	//g_Shader.LinkShaders();


 	ObjParser parser;
	MultiMeshData data;


	// CAMERA
	//MAINCAMERANODE->LocalTransform().Position = Vec3(0.f, 8.f, 15.f);
	//MAINCAMERANODE->LocalTransform().Position = Vec3(0.f, 0.f, 10.f);
	//MAINCAMERANODE->LocalTransform().Rotation = Quaternion(180.f, Vec3::Up());

	//modelNode->LocalTransform().Scale = Vec3(0.5f);
	//m_OGL_Scene.CenterCamera(m_Model.GetMin(), m_Model.GetMax(), 60.f);
	{
		CameraFlyAroundController* controller = COMPONENTINCUBATOR->Create<CameraFlyAroundController>();
		controller->Attach(MAINCAMERANODE);
	}


	// MODELS
	{
		std::string name = "../Resources/MODELS/_ciri_model/ciri";
		if (!std::fstream(name + ".mys").good())
		{
			parser.ParseFile(name + ".obj");
			data = parser.GenerateMeshData(true);
			data.Serialize(name + ".mys");
		}
		else
		{
			//parser.LoadDebugBinary("debug.mys");
			//data = parser.GenerateMeshData(true);
			data.Deserialize(name + ".mys");
		}

		Node* modelNode = ROOTNODE->CreateChild();
		modelNode->LocalTransform().Position = Vec3(0.f, 0.f, 0.f);
		{
			RotateAround* controller = COMPONENTINCUBATOR->Create<RotateAround>();
			//controller->Attach(modelNode);
		}

		Node* offsetNode = modelNode->CreateChild();
		offsetNode->LocalTransform().Position = Vec3(0.f, 0.f, 0.f);
		OGL_RenderObject* model = COMPONENTINCUBATOR->Create<OGL_RenderObject>();
		model->AddMultiMesh(data, &g_Shader);
		model->Attach(offsetNode);

		// NOTE: We want the camera to focus on this object
		modelNode->LocalTransform().Scale = Vec3(0.02f);
		DEVICE->CurrentScene()->MainCamera()->CenterOnBounds(model->Min(), model->Max());
	}

	{
		std::string name = "../Resources/MODELS/sphere";
		if (!std::fstream(name + ".mys").good())
		{
			parser.ParseFile(name + ".obj");
			data = parser.GenerateMeshData(true);
			data.Serialize(name + ".mys");
		}
		else
		{
			data.Deserialize(name + ".mys");
		}

		Node* modelNode = ROOTNODE->CreateChild();
		modelNode->LocalTransform().Position = Vec3(0.f, 10.f, 0.f);
		{
			RotateAround* controller = COMPONENTINCUBATOR->Create<RotateAround>();
			controller->speed = 50.f;
			controller->Attach(modelNode);
		}

		Node* offsetNode = modelNode->CreateChild();
		offsetNode->LocalTransform().Position = Vec3(0.f, 0.f, 10.f);
		{
			OGL_RenderObject* model = COMPONENTINCUBATOR->Create<OGL_RenderObject>();
			model->AddMultiMesh(data, &g_Shader);
			model->Attach(offsetNode);
			{
				RotateAround* controller = COMPONENTINCUBATOR->Create<RotateAround>();
				controller->Attach(offsetNode);
				controller->speed = -140.f;
			}
		}

		Node* anotherNode = offsetNode->CreateChild();
		anotherNode->LocalTransform().Position = Vec3(0.f, 0.f, 3.f);
		{
			OGL_RenderObject* model = COMPONENTINCUBATOR->Create<OGL_RenderObject>();
			model->AddMultiMesh(data, &g_Shader);
			model->Attach(anotherNode);
			{
				RotateAround* controller = COMPONENTINCUBATOR->Create<RotateAround>();
				controller->Attach(anotherNode);
				controller->speed = 90.f;
			}
		}
	}

	// SKYBOX
	OGL_Skybox* skybox = COMPONENTINCUBATOR->Create<OGL_Skybox>();
	//skybox->SetTexturePath(ISkybox::BACK, "../Resources/TEXTURES/lake_sb/lake1_bk.JPG");
	//skybox->SetTexturePath(ISkybox::FRONT, "../Resources/TEXTURES/lake_sb/lake1_ft.JPG");
	//skybox->SetTexturePath(ISkybox::LEFT, "../Resources/TEXTURES/lake_sb/lake1_lf.JPG");
	//skybox->SetTexturePath(ISkybox::RIGHT, "../Resources/TEXTURES/lake_sb/lake1_rt.JPG");
	//skybox->SetTexturePath(ISkybox::TOP, "../Resources/TEXTURES/lake_sb/lake1_up.JPG");
	//skybox->SetTexturePath(ISkybox::BOTTOM, "../Resources/TEXTURES/lake_sb/lake1_dn.JPG");
	skybox->SetTexturePath(ISkybox::BACK, "../Resources/TEXTURES/yokohama/posz.jpg");
	skybox->SetTexturePath(ISkybox::FRONT, "../Resources/TEXTURES/yokohama/negz.jpg");
	skybox->SetTexturePath(ISkybox::LEFT, "../Resources/TEXTURES/yokohama/negx.jpg");
	skybox->SetTexturePath(ISkybox::RIGHT, "../Resources/TEXTURES/yokohama/posx.jpg");
	skybox->SetTexturePath(ISkybox::TOP, "../Resources/TEXTURES/yokohama/posy.jpg");
	skybox->SetTexturePath(ISkybox::BOTTOM, "../Resources/TEXTURES/yokohama/negy.jpg");
	skybox->AllocateResources();
	OGL_Shader* skyboxShader = new OGL_Shader();
	skyboxShader->LoadShaderAndCompile("../Resources/SHADERS/Skybox.vs", GL_VERTEX_SHADER);
	skyboxShader->LoadShaderAndCompile("../Resources/SHADERS/Skybox.fs", GL_FRAGMENT_SHADER);
	skyboxShader->LinkShaders();

	parser.ParseFile("../Resources/MODELS/SKYBOX/Skybox.obj");
	data = parser.GenerateMeshData();
	skybox->SetBoxMesh(data.m_Meshes[0], skyboxShader);
	DEVICE->CurrentScene()->SetSkybox(skybox);


	// LIGHTS
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
	//light2->Attach(lightNode);
	light2->m_Type = Light::DIRECTIONAL;
	light2->m_Ia = Vec3(0.f, 0.f, 0.f);
	light2->m_Id = Vec3(1.f, 0.f, 0.f);
	light2->m_Is = Vec3(1.f, 0.f, 0.f);
	light2->m_Direction = Vec3(0.f, 1.f, -1.f);

	lightNode = NODEINCUBATOR->Create();
	Light* light3 = COMPONENTINCUBATOR->Create<Light>();
	//light3->Attach(lightNode);
	light3->m_Type = Light::SPOT;
	light3->m_Ia = Vec3(0.f, 0.f, 0.f);
	light3->m_Id = Vec3(0.f, 1.f, 0.f);
	light3->m_Is = Vec3(0.f, 1.f, 0.f);
	light3->m_Direction = Vec3(0.f, -0.8f, 1.f);
	light3->m_Position = Vec3(0.f, 20.f, -5.f);
	light3->m_Cutoff = 25.f;
}

#endif /*__TEST_SCENE_HPP__*/