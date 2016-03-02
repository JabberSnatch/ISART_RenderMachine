#include <Windows.h>

#include <iostream>

#if defined(_WIN64)
#pragma comment(lib, "opengl32.lib")
#elif defined(_WIN32)
#pragma comment(lib, "opengl32.lib")
#endif

#include "IRenderContext.hpp"

#include "OGL_RenderContext.hpp"
#include "Device.hpp"


static const wchar_t AppName[] = { L"RenderMachine" };
static const wchar_t WindowName[] = { L"YOLORenderSQUAD" };

enum E_RENDERER { OPENGL, D3D11, COUNT };

LRESULT WINAPI		DispatchMessages(HWND _hWnd, UINT _msg, WPARAM _wParam, LPARAM _lParam);
IRenderContext*		CreateContext(E_RENDERER _type, HWND _window);


static Device g_Device;

int WINAPI 
WinMain(HINSTANCE _hInstance, HINSTANCE _hPrevInstance, LPSTR _lpCmdLine, int _nCmdShow)
{
	WNDCLASS wc = { 0 };
	HWND hWnd;

	{
		{
			wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
			wc.lpfnWndProc = DispatchMessages;
			wc.hInstance = _hInstance;
			wc.lpszClassName = (LPCWSTR)AppName;
		}
		RegisterClass(&wc);

		const int width = 1280;
		const int height = 740;
		RECT canvasRect = { 0, 0, width, height };
		DWORD style = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
		AdjustWindowRect(&canvasRect, style, FALSE);

		HWND hWnd = CreateWindowEx(
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

		IRenderContext* context = CreateContext(OPENGL, hWnd);
	
		g_Device.Initialize(width, height);
		g_Device.SetRenderContext(context);
		g_Device.OGL_SETUP();
	}


	// RUN LOOP
	MSG msg = { 0 };
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		g_Device.Update(1. / 60.);
		g_Device.Render();
	}


	g_Device.Shutdown();

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
		g_Device.SetDimensions(position->cx - 16, position->cy - 19);
	} break;
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

