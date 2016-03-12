#include <Windows.h>

#include "imgui.h"
#include <windowsx.h>

#include <iostream>

#if defined(_WIN64)
#pragma comment(lib, "opengl32.lib")
#elif defined(_WIN32)
#pragma comment(lib, "opengl32.lib")
#endif


#include "ImGui_OGL_RenderSystem.hpp"

#include "IRenderer.hpp"
#include "IRenderContext.hpp"

#include "OGL_Renderer.hpp"
#include "OGL_RenderContext.hpp"
#include "Device.hpp"


static const wchar_t	AppName[] = { L"RenderMachine" };
static const wchar_t	WindowName[] = { L"YOLORenderSQUAD" };
static IRenderer*		g_Renderer = nullptr;

enum E_RENDERER { OPENGL, D3D11, COUNT };

LRESULT WINAPI		DispatchMessages(HWND _hWnd, UINT _msg, WPARAM _wParam, LPARAM _lParam);
IRenderer*			CreateRenderer(E_RENDERER _type);
IRenderContext*		CreateContext(E_RENDERER _type, HWND _window);
void				ImGui_RenderWrapper(ImDrawData* _data);
void				INIT_TEST_SCENE();

int WINAPI 
WinMain(HINSTANCE _hInstance, HINSTANCE _hPrevInstance, LPSTR _lpCmdLine, int _nCmdShow)
{
	WNDCLASS wc = { 0 };
	HWND hWnd;

	const int width = 1280;
	const int height = 740;
	{
		{
			wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
			wc.lpfnWndProc = DispatchMessages;
			wc.hInstance = _hInstance;
			wc.lpszClassName = (LPCWSTR)AppName;
		}
		RegisterClass(&wc);

		RECT canvasRect = { 0, 0, width, height };
		DWORD style = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
		AdjustWindowRect(&canvasRect, style, FALSE);

		hWnd = CreateWindowEx(
			0,
			(LPCWSTR)AppName,
			(LPCWSTR)WindowName,
			style,
			CW_USEDEFAULT, CW_USEDEFAULT,
			canvasRect.right - canvasRect.left, canvasRect.bottom - canvasRect.top,
			0, 0, _hInstance, 0);

		if (!hWnd)
		{
			printf("Window creation failed.\n");
			goto __SHUTDOWN;
		}
	}

	IRenderContext* context = CreateContext(OPENGL, hWnd);
	g_Renderer = CreateRenderer(OPENGL);
	
	DEVICE->Initialize(width, height);
	DEVICE->SetRenderContext(context);
	DEVICE->SetRenderer(g_Renderer);
	//DEVICE->OGL_SETUP();

	INIT_TEST_SCENE();

	{
		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2((float)width, (float)height);
		io.DeltaTime = 1.f / 60.f;
		io.KeyMap[ImGuiKey_UpArrow] = VK_UP;
		io.KeyMap[ImGuiKey_DownArrow] = VK_DOWN;
		io.KeyMap[ImGuiKey_LeftArrow] = VK_LEFT;
		io.KeyMap[ImGuiKey_RightArrow] = VK_RIGHT;
		io.KeyMap[ImGuiKey_Tab] = VK_TAB;
		io.KeyMap[ImGuiKey_Home] = VK_HOME;
		io.KeyMap[ImGuiKey_End] = VK_END;
		io.KeyMap[ImGuiKey_Delete] = VK_DELETE;
		io.KeyMap[ImGuiKey_Backspace] = VK_BACK;
		io.KeyMap[ImGuiKey_Enter] = VK_RETURN;
		io.KeyMap[ImGuiKey_Escape] = VK_ESCAPE;
		io.KeyMap[ImGuiKey_A] = 'A';
		io.KeyMap[ImGuiKey_C] = 'C';
		io.KeyMap[ImGuiKey_V] = 'V';
		io.KeyMap[ImGuiKey_X] = 'X';
		io.KeyMap[ImGuiKey_Y] = 'Y';
		io.KeyMap[ImGuiKey_Z] = 'Z';
		io.RenderDrawListsFn = ImGui_OGL_RenderDrawLists;

		ImGui_OGL_InitResources();
	}


	// RUN LOOP
	MSG msg = { 0 };
	while (msg.message != WM_QUIT)
	{
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		{
			ImGuiIO& io = ImGui::GetIO();

			BYTE keystate[256];
			GetKeyboardState(keystate);
			for (int i = 0; i < 256; ++i)
				io.KeysDown[i] = (keystate[i] & 0x80);
			io.KeyCtrl = (keystate[VK_CONTROL] & 0x80);
			io.KeyAlt = (keystate[VK_MENU] & 0x80);
			io.KeyShift = (keystate[VK_SHIFT] & 0x80);

			iVec2& mousePos = INPUT->MousePosition();
			io.MousePos = ImVec2(mousePos.x, mousePos.y);
			io.MouseDown[0] = INPUT->IsMouseButtonDown(EMOUSE_BUTTON::MOUSE_LEFT);
			io.MouseDown[1] = INPUT->IsMouseButtonDown(EMOUSE_BUTTON::MOUSE_RIGHT);
			io.MouseDown[2] = INPUT->IsMouseButtonDown(EMOUSE_BUTTON::MOUSE_MIDDLE);
			io.MouseWheel = 0.f;
		}
		ImGui::NewFrame();
		ImGui::ShowTestWindow();

		DEVICE->Update(1. / 60.);
		DEVICE->Render();
		
		ImGui::Render();
		DEVICE->SwapBuffers();
	}


	ImGui_OGL_FreeResources();
	DEVICE->Shutdown();

__SHUTDOWN:
	UnregisterClass(AppName, wc.hInstance);

	return 0;
}


int
main(int argc, char** argv)
{
	return WinMain(GetModuleHandle(nullptr), nullptr, nullptr, SW_SHOW);
}


LRESULT WINAPI
DispatchMessages(HWND _hWnd, UINT _msg, WPARAM _wParam, LPARAM _lParam)
{
	switch (_msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_WINDOWPOSCHANGED:
	{
		WINDOWPOS* position = (WINDOWPOS*)_lParam;
		DEVICE->SetDimensions(position->cx - 16, position->cy - 19);
	} break;
	case WM_KEYDOWN:
	case WM_KEYUP:
	{
		if (_wParam >= '0' && _wParam <= '9')
		{
			INPUT->SetKeyValue((EKEY_CODE)(EKEY_CODE::KEY_0 + (_wParam - '0')), (_msg == WM_KEYDOWN));
		}
		else if (_wParam >= 'A' && _wParam <= 'Z')
		{
			INPUT->SetKeyValue((EKEY_CODE)(EKEY_CODE::KEY_A + (_wParam - 'A')), (_msg == WM_KEYDOWN));
		}
		else
		{
			switch (_wParam)
			{
			case VK_UP:
				INPUT->SetKeyValue(EKEY_CODE::KEY_UP, (_msg == WM_KEYDOWN));
				break;
			case VK_DOWN:
				INPUT->SetKeyValue(EKEY_CODE::KEY_DOWN, (_msg == WM_KEYDOWN));
				break;
			case VK_LEFT:
				INPUT->SetKeyValue(EKEY_CODE::KEY_LEFT, (_msg == WM_KEYDOWN));
				break;
			case VK_RIGHT:
				INPUT->SetKeyValue(EKEY_CODE::KEY_RIGHT, (_msg == WM_KEYDOWN));
				break;
			case VK_SPACE:
				INPUT->SetKeyValue(EKEY_CODE::KEY_SPACE, (_msg == WM_KEYDOWN));
				break;
			case VK_RETURN:
				INPUT->SetKeyValue(EKEY_CODE::KEY_RETURN, (_msg == WM_KEYDOWN));
				break;
			case VK_CONTROL:
				INPUT->SetKeyValue(EKEY_CODE::KEY_CTRL, (_msg == WM_KEYDOWN));
				break;
			case VK_SHIFT:
				INPUT->SetKeyValue(EKEY_CODE::KEY_SHIFT, (_msg == WM_KEYDOWN));
				break;
			case VK_MENU:
				INPUT->SetKeyValue(EKEY_CODE::KEY_ALT, (_msg == WM_KEYDOWN));
				break;
			case VK_ESCAPE:
				INPUT->SetKeyValue(EKEY_CODE::KEY_ESCAPE, (_msg == WM_KEYDOWN));
				break;
			case VK_TAB	:
				INPUT->SetKeyValue(EKEY_CODE::KEY_TAB, (_msg == WM_KEYDOWN));
				break;
			case VK_HOME:
				INPUT->SetKeyValue(EKEY_CODE::KEY_HOME, (_msg == WM_KEYDOWN));
				break;
			case VK_END:
				INPUT->SetKeyValue(EKEY_CODE::KEY_END, (_msg == WM_KEYDOWN));
				break;
			case VK_DELETE:
				INPUT->SetKeyValue(EKEY_CODE::KEY_DELETE, (_msg == WM_KEYDOWN));
				break;
			case VK_BACK:
				INPUT->SetKeyValue(EKEY_CODE::KEY_BACKSPACE, (_msg == WM_KEYDOWN));
				break;
			}
		}
	} break;
	case WM_CHAR:
	{
		ImGuiIO& io = ImGui::GetIO();
		io.AddInputCharactersUTF8((char*)&_wParam);
	} break;
	case WM_MOUSEMOVE:
	{
		int x = GET_X_LPARAM(_lParam);
		int y = GET_Y_LPARAM(_lParam);
		INPUT->SetMousePosition(x, y);
	} break;
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
		INPUT->SetMouseButtonValue(EMOUSE_BUTTON::MOUSE_LEFT, (_msg == WM_LBUTTONDOWN));
		break;
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
		INPUT->SetMouseButtonValue(EMOUSE_BUTTON::MOUSE_RIGHT, (_msg == WM_RBUTTONDOWN));
		break;
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
		INPUT->SetMouseButtonValue(EMOUSE_BUTTON::MOUSE_MIDDLE, (_msg == WM_MBUTTONDOWN));
		break;
	}

	return DefWindowProc(_hWnd, _msg, _wParam, _lParam);
}


IRenderContext*		
CreateContext(E_RENDERER _type, HWND _window)
{
	IRenderContext* result = nullptr;

	switch (_type)
	{
	case OPENGL:
	{
		OGL_RenderContext* context = new OGL_RenderContext();
		context->Initialize(_window);
		result = (IRenderContext*)context;
	} break;
	case D3D11:
		break;
	default: break;
	}

	return result;
}


IRenderer*
CreateRenderer(E_RENDERER _type)
{
	IRenderer* result = nullptr;

	switch (_type)
	{
	case OPENGL:
	{
		OGL_Renderer* context = new OGL_Renderer();
		context->Initialize();
		result = (IRenderer*)context;
	} break;
	case D3D11:
		break;
	default: break;
	}

	return result;
}


void
ImGui_RenderWrapper(ImDrawData* _data)
{
	g_Renderer->ImGui_RenderDrawLists(_data);
}


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
		data = parser.GenerateMeshData();
		data.Serialize(name + ".mys");
	}
	else
		data.Deserialize(name + ".mys");


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


