#define FREEGLUT_LIB_PRAGMAS 0
#include <glew/include/GL/glew.h>
#include <freeglut/include/GL/freeglut.h>

#if defined(_WIN64)
#pragma comment(lib, "x64/freeglut.lib")
#elif defined(_WIN32)
#pragma comment(lib, "freeglut.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32.lib")
#endif

#define BUFFER_OFFSET(offset) ((GLvoid*)(offset))

#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

#include "ObjParser.hpp"
#include "Matrix.hpp"
#include "OGL_Model.hpp"
#include "OGL_Mesh.hpp"
#include "OGL_Shader.hpp"
#include "OGL_Scene.hpp"
#include "OGL_TextureLoader.hpp"
static OGL_Scene g_scene;
static OGL_Mesh* g_mesh;
static OGL_Model g_Model;
static OGL_Model g_nxtModel;
static OGL_Shader* g_shader;
static OGL_Shader* g_CommonShader;

#include "OGL_Object.h"
static OGL_Object g_object;
static GLuint g_TextureID;

static float g_Time = 0.f;

void Update()
{
	glutPostRedisplay();

	g_Time += 1.f / 60.f;
	g_object.Update(g_Time);

	g_Model.GetTransform().Rotation += Vec3::Up() * (1.f / 60.f) * -45.f;
	//g_mesh->GetTransform().Rotation += Vec3::Up() * (1.f / 60.f) * 45.f;
	//g_mesh->GetTransform().Position = Vec3::Up() * cos(g_Time);

	g_scene.GetCameraTransform().Rotation += Vec3::Up() * (1.f / 60.f) * -15.f;
}


void Render()
{
	g_object.BindFBO();
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);

	//g_object.EnableTexture(g_TextureID);
	
	//g_object.Render();
	//g_object.RenderInstanced(1);

	//g_object.BlitAndUnbindFBO();
	//g_object.EnableTexture(0);

	g_scene.Render();

	glutSwapBuffers();
}

void Initialize()
{
	cout << "Version OpenGL : " << glGetString(GL_VERSION) << endl;
	cout << "Fabriquant : " << glGetString(GL_VENDOR) << endl;
	cout << "Pilote : " << glGetString(GL_RENDERER) << endl;

	glewInit();

	GLint extCount;
	glGetIntegerv(GL_NUM_EXTENSIONS, &extCount);
	for (auto index = 0; index < extCount; ++index) {
		//cout << "Extension[" << glGetStringi(GL_EXTENSIONS, index) << "]" << endl;
	}

	GLfloat triangles[] = {
		-.69f, .4f, .4f,		1.f, 1.f, 0.f, 1.f,		.0f, 1.f,
		0.f, -.8f, 0.f,			0.f, 1.f, 1.f, 1.f,		.5f, 0.f,
		0.69f, .4f, .4f,		1.f, 0.f, 1.f, 1.f,		1.f, 1.f,
		0.f, .4f, -.8f,			1.f, 1.f, 1.f, 1.f,		0.f, 0.f,
		0.f, 0.f, 0.29f,		.5f, .5f, .5f, 1.f,		.5f, .66f
	};

	// A = { .69, .4, -.4}
	// B = { -.69, .4, -.4}
	// C = { 0., -.8, 0.}
	// D = { 0., .4, -.69}

	GLuint indices[] = {
		0, 1, 4,
		0, 4, 2,
		1, 2, 4,
		1, 3, 2,
		0, 3, 1,
		0, 2, 3
	};

	GLint sizes[] = {
		3, 4, 2
	};


	g_shader = new OGL_Shader();
	g_shader->LoadShaderAndCompile("IlluminationShader.vs", GL_VERTEX_SHADER);
	g_shader->LoadShaderAndCompile("IlluminationShader.fs", GL_FRAGMENT_SHADER);
	g_shader->LinkShaders();


	ObjParser parser;
	MultiMeshData data;
	
	//std::string name = "_zero_model/zero";
	std::string name = "_ciri_model/ciri";
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

	g_Model.AddMultiMesh(data, g_shader);
	g_scene.AddModel(g_Model);
	
	static GLfloat projectionMatrix[16];
	OGL_Object::ComputePerspectiveProjectionFOV(projectionMatrix, 60.f, glutGet(GLUT_SCREEN_WIDTH) / glutGet(GLUT_SCREEN_HEIGHT), .1f, 1000.f);
	
	g_scene.CreateBuffers();
	g_scene.SetPerspectiveMatrix(projectionMatrix);
	g_scene.GetCameraTransform().Position = Vec3(0.f, 8.f, 15.f);
	//g_scene.GetCameraTransform().Position = Vec3(0.f, 0.f, 2.f);

	g_Model.GetTransform().Scale = Vec3(0.02f);
	//g_Model.GetTransform().Rotation = Vec3::Up() * 180.f;
	//g_Model.GetTransform().Scale = Vec3(7.f);
	g_scene.CenterCamera(g_Model.GetMin(), g_Model.GetMax(), 60.f);


	OGL_Light light0(OGL_Light::DIRECTIONAL);
	light0.m_Ia = Vec3(.25f, .2f, .15f);
	light0.m_Id = Vec3(0.8f, 0.75f, 0.75f);
	light0.m_Is = Vec3(.8f, .8f, .7f);
	light0.m_Direction = Vec3(0.f, 0.f, -1.f);

	OGL_Light light1(OGL_Light::POINT);
	light1.m_Ia = Vec3(.0f, .0f, 0.f);
	light1.m_Id = Vec3(0.f, 0.f, 1.f);
	light1.m_Is = Vec3(.0f, .0f, 1.f);
	light1.m_Position = Vec3(0.f, 5.f, -5.f);

	OGL_Light light2(OGL_Light::DIRECTIONAL);
	light2.m_Ia = Vec3(0.f, 0.f, 0.f);
	light2.m_Id = Vec3(1.f, 0.f, 0.f);
	light2.m_Is = Vec3(1.f, 0.f, 0.f);
	light2.m_Direction = Vec3(0.f, 0.f, -1.f);

	OGL_Light light3(OGL_Light::SPOT);
	light3.m_Ia = Vec3(0.f, 0.f, 0.f);
	light3.m_Id = Vec3(0.f, 1.f, 0.f);
	light3.m_Is = Vec3(0.f, 1.f, 0.f);
	light3.m_Direction = Vec3(0.f, -.75f, 1.f);
	light3.m_Position = Vec3(0.f, 20.f, -5.f);
	light3.m_Cutoff = 15.f;


	g_scene.AddLight(light1);
	g_scene.AddLight(light2);
	g_scene.AddLight(light3);


	// TRASH
	/*
	g_mesh = new OGL_Mesh();

	g_mesh->SetPolyCount(6);
	g_mesh->SetVertexSize(9);
	g_mesh->SetVerticesCount(5);
	g_mesh->SetData(triangles);
	g_mesh->SetIndices(indices);
	g_mesh->SetAttribSizes(sizes, 3);

	g_shader->LoadShaderAndCompile("shaderDuQ.vs", GL_VERTEX_SHADER);
	g_shader->LoadShaderAndCompile("fsDuQ.fs", GL_FRAGMENT_SHADER);
	g_shader->LinkShaders();

	g_scene.AddMesh(g_mesh);


	g_object.LoadShaderAndCompile("shaderDuQ.vs", GL_VERTEX_SHADER);
	g_object.LoadShaderAndCompile("fsDuQ.fs", GL_FRAGMENT_SHADER);
	//g_object.LoadShaderAndCompile("IlluminationShader.vs", GL_VERTEX_SHADER);
	//g_object.LoadShaderAndCompile("IlluminationShader.fs", GL_FRAGMENT_SHADER);
	g_object.LinkShaders();

	g_object.Initialize();

	g_object.SetData(triangles, 9 * 5);
	g_object.SetIndices(indices, 3 * 6);
	g_object.SetAttribSizes(sizes, 3);
	g_object.SetPolyCount(6);
	g_object.SetVerticesSize(9);
	g_object.SetVerticesCount(5);
	

	g_object.CreateVBO();
	g_object.CreateIBO();
	g_object.CreateVAO();
	g_object.CreateFBO(800, 800);

	g_TextureID = g_object.CreateTexture("txt_mur.tga");
	*/
}


void Terminate()
{
	g_object.FreeResources();
	OGL_TextureLoader::Kill();
}

int main(int argc, char* argv[])
{

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(800, 800);
	glutCreateWindow("YOLO");
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);

	glutIdleFunc(Update);
	glutDisplayFunc(Render);

	//glEnable(GL_SCISSOR_TEST);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(.5f, .5f, .5f, 1.f);

	Initialize();
	glutMainLoop();
	Terminate();

	return 0;
}