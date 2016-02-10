// ---------------------------------------------------------------------------
//
// Triangle dans une fenetre version DirectX11 par Malek Bengougam
//
// Exemple le plus basique qu'il soit en utilisant le moins de dependances
//
// ---------------------------------------------------------------------------

// C++
#include <cstddef>
#include <cstdio>
#include <string>
#include <fstream>

#if _DEBUG
//#define ENABLE_ERROR_CHECKING
#endif

// DirectX 11
#include <d3d11.h>
#include <d3dcompiler.h>
#if defined(ENABLE_ERROR_CHECKING)
#include <DxErr.h>
#endif
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p) = nullptr; } }
#endif

#if defined(ENABLE_ERROR_CHECKING)
void DX_CHECK(HRESULT hr)
{
	if FAILED(hr) {
		printf("[Erreur DirectX] %s:  %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		DebugBreak();
	}
}
#else
#define DX_CHECK()
#endif

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "DXGI.lib")
#if defined(ENABLE_ERROR_CHECKING)
#pragma comment(lib, "dxerr.lib")
#endif

// ---

static ID3D11Device*			g_D3dDevice = nullptr;
static ID3D11DeviceContext*		g_D3dDeviceImmediateContext = nullptr;
static ID3D11RasterizerState*	g_D3dRasterizerState = nullptr;
static IDXGISwapChain*			g_SwapChain = nullptr;

static ID3D11RenderTargetView*	g_MainRenderTargetView;
static ID3D11DepthStencilView*	g_DepthStencilView;

static ID3D10Blob*				g_ShaderBlob = nullptr;
static ID3D11VertexShader*		g_VertexShader = nullptr;
static ID3D11PixelShader*		g_PixelShader = nullptr;

static ID3D11Buffer*			g_VertexBuffer = nullptr;
static ID3D11Buffer*			g_IndexBuffer = nullptr;
static ID3D11Buffer*			g_MatrixBuffer = nullptr;
static ID3D11InputLayout*		g_Layout = nullptr;


static const D3D_FEATURE_LEVEL featureLevelArray[] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0, D3D_FEATURE_LEVEL_9_3 };
static int g_FeatureLevelIndex = 0;
static const char* g_VsTarget[] = { "vs_5_0", "vs_4_1", "vs_4_0", "vs_3_0" };
static const char* g_PsTarget[] = { "ps_5_0", "ps_4_1", "ps_4_0", "ps_3_0" };

// ---


// ---

#include "Matrix.hpp"
#include "D3D_Object.hpp"

D3D_Object::MatrixBuffer g_Matrices;

float g_deltaTime = 0.f;
D3D_Object g_object;

auto
GetFileContent(std::string _path) -> char*
{
	std::fstream sourceFile(_path, std::fstream::in);
	sourceFile.seekg(0, sourceFile.end);
	std::streamoff length = sourceFile.tellg();
	sourceFile.seekg(0, sourceFile.beg);

	char* buffer = new char[length];
	sourceFile.get(buffer, length, '\0');

	return buffer;
}


#define PI 3.14159265359
auto
ComputePerspectiveProjectionFOV(float* o_matrix, float _fov, float _aspectRatio, float _near, float _far) -> void
{
	float yscale = 1.0f / tan(_fov * (PI / 360.));
	float xscale = yscale / _aspectRatio;

	float depth = _far - _near;
	float q = (_far) / depth;
	float qn = q * -_near;

	float result[] =
	{
		xscale, 0.f, 0.f, 0.f,
		0.f, yscale, 0.f, 0.f,
		0.f, 0.f, q, 1.f,
		0.f, 0.f, qn, 0.f
	};

	memcpy(o_matrix, result, 16 * sizeof(float));
}


HRESULT InitialiseDirect3DDevice(HWND hWnd)
{
	HRESULT hr = S_OK;

	// definition de la swap chain
	DXGI_SWAP_CHAIN_DESC sd = { 0 };
	sd.Windowed = TRUE;
	sd.OutputWindow = hWnd;
	sd.BufferCount = 1;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sd.SampleDesc.Count = 1;

	UINT createDeviceFlag = 0;
#ifdef _DEBUG
	createDeviceFlag |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	D3D_FEATURE_LEVEL featureLevel;
	hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE
									   , nullptr, createDeviceFlag, featureLevelArray
									   , _countof(featureLevelArray), D3D11_SDK_VERSION
									   , &sd, &g_SwapChain, &g_D3dDevice, &featureLevel
									   , &g_D3dDeviceImmediateContext);
	if FAILED(hr)
		return hr;

	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(D3D11_BLEND_DESC));
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	ID3D11BlendState* blendState;
	hr = g_D3dDevice->CreateBlendState(&blendDesc, &blendState);
	g_D3dDeviceImmediateContext->OMSetBlendState(blendState, nullptr, 0xff);

	D3D11_RASTERIZER_DESC rasterizerDesc;
	rasterizerDesc.CullMode = D3D11_CULL_FRONT;
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.FrontCounterClockwise = TRUE;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0.0f;
	rasterizerDesc.SlopeScaledDepthBias = 0.0f;
	rasterizerDesc.MultisampleEnable = FALSE;
	rasterizerDesc.ScissorEnable = FALSE;
	rasterizerDesc.DepthClipEnable = FALSE;
	rasterizerDesc.AntialiasedLineEnable = FALSE;
	g_D3dDevice->CreateRasterizerState(&rasterizerDesc, &g_D3dRasterizerState);

	UINT backbufferWidth, backbufferHeight;
	// Le back buffer existe deja, il faut seulement creer une Render Target View (RTV)
	ID3D11Texture2D* pBackBuffer;
	hr = g_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
	if FAILED(hr)
		return hr;
	D3D11_TEXTURE2D_DESC backbuffer_texture_desc;
	pBackBuffer->GetDesc(&backbuffer_texture_desc);
	backbufferWidth = backbuffer_texture_desc.Width;
	backbufferHeight = backbuffer_texture_desc.Height;
	hr = g_D3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_MainRenderTargetView);
	if FAILED(hr)
		return hr;
	// le GetBuffer() incremente le compteur de reference il faut donc faire un Release()
	pBackBuffer->Release();


	ID3D11Texture2D* depthStencil;

	D3D11_TEXTURE2D_DESC depthBufferDesc = { 0 };
	depthBufferDesc.Width = backbufferWidth;
	depthBufferDesc.Height = backbufferHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1; // Verifier la doc
	depthBufferDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	hr = g_D3dDevice->CreateTexture2D(&depthBufferDesc, nullptr, &depthStencil);
	if FAILED(hr)
		return hr;

	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = { 0 };
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilDesc.StencilEnable = false;

	ID3D11DepthStencilState* DSState;
	g_D3dDevice->CreateDepthStencilState(&depthStencilDesc, &DSState);
	g_D3dDeviceImmediateContext->OMSetDepthStencilState(DSState, 1);

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	ZeroMemory(&dsvDesc, sizeof(dsvDesc));
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;

	hr = g_D3dDevice->CreateDepthStencilView(depthStencil, &dsvDesc, &g_DepthStencilView);
	SAFE_RELEASE(depthStencil);
	if FAILED(hr)
		return hr;


	g_D3dDeviceImmediateContext->OMSetRenderTargets(1, &g_MainRenderTargetView, g_DepthStencilView);


	D3D11_VIEWPORT vp;
	ZeroMemory(&vp, sizeof D3D11_VIEWPORT);
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.Width = (float)backbufferWidth;
	vp.Height = (float)backbufferHeight;
	g_D3dDeviceImmediateContext->RSSetViewports(1, &vp);


	g_object.Initialize(g_D3dDevice, g_D3dDeviceImmediateContext);
	g_object.AppendVertexAttribDesc("POSITION", DXGI_FORMAT_R32G32B32_FLOAT);
	g_object.AppendVertexAttribDesc("COLOR", DXGI_FORMAT_R32G32B32A32_FLOAT);
	//g_object.AppendVertexAttribDesc("TEXCOORD0", DXGI_FORMAT_R32G32_FLOAT);

	g_object.LoadShaderAndCompile("DefaultVertexShader.vs", D3D_Object::VertexShader);
	g_object.LoadShaderAndCompile("DefaultPixelShader.ps", D3D_Object::PixelShader);

	static float pVertices[] = {
		/*
		-.69f, .4f, 0.f,	1.f, 1.f, 0.f, 1.f,
		0.f, -.8f, 0.f,		0.f, 1.f, 1.f, 1.f,
		0.69f, .4f, 0.f,	1.f, 0.f, 1.f, 1.f
		*/

		- .69f, .4f, .4f,		1.f, 1.f, 0.f, 1.f,		//.0f, 1.f,
		0.f, -.8f, 0.f,			0.f, 1.f, 1.f, 1.f,		//.5f, 0.f,
		0.69f, .4f, .4f,		1.f, 0.f, 1.f, 1.f,		//1.f, 1.f,
		0.f, .4f, -.8f,			1.f, 1.f, 1.f, 1.f,		//0.f, 0.f,
		0.f, 0.f, 0.29f,		.5f, .5f, .5f, 1.f		//,.5f, .66f
	};

	static unsigned long pIndices[] = {
		//0, 2, 1

		0, 1, 4,
		0, 4, 2,
		1, 2, 4,

		1, 3, 2,
		0, 3, 1,
		0, 2, 3
	};

	g_object.SetVerticesCount(5);
	g_object.SetVerticesSize(7);
	g_object.SetPolyCount(6);
	
	g_object.SetData(pVertices);
	g_object.SetIndices(pIndices);

	g_object.CreateVertexBuffer();
	g_object.CreateIndexBuffer();
	g_object.CreateMatrixBuffer();

	return hr;
}

void ShutdownDirect3DDevice()
{
	if (g_D3dDeviceImmediateContext) {
		g_D3dDeviceImmediateContext->ClearState();
	}

	SAFE_RELEASE(g_DepthStencilView);

	SAFE_RELEASE(g_ShaderBlob);
	SAFE_RELEASE(g_PixelShader);
	SAFE_RELEASE(g_VertexShader);

	SAFE_RELEASE(g_SwapChain);
	SAFE_RELEASE(g_D3dDeviceImmediateContext);
	SAFE_RELEASE(g_D3dDevice);
}

// ---

void Render()
{
	g_deltaTime += 1.f / 3000.f;

	static const float clearColor[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	g_D3dDeviceImmediateContext->ClearRenderTargetView(g_MainRenderTargetView
													   , clearColor);
	g_D3dDeviceImmediateContext->ClearDepthStencilView(g_DepthStencilView, D3D11_CLEAR_DEPTH, 1.f, 0);
	g_D3dDeviceImmediateContext->RSSetState(g_D3dRasterizerState);
	

	ComputePerspectiveProjectionFOV(g_Matrices.m_ProjectionMatrix, 90.f, 1.f, .1f, 1000.f);

	static float cameraMatrix[] = { 1.f, 0.f, 0.f, 0.f,
							 0.f, 1.f, 0.f, 0.f,
							 0.f, 0.f, 1.f, 0.f,
							 0.f, 0.f, 2.f, 1.f };
	memcpy(g_Matrices.m_ViewMatrix, cameraMatrix, 16 * sizeof(float));

	Matrix::identity(g_Matrices.m_WorldMatrix);
	float rotationY[] = { cos(g_deltaTime), 0.f, sin(g_deltaTime), 0.f,
							0.f, 1.f, 0.f, 0.f,
							-sin(g_deltaTime), 0.f, cos(g_deltaTime), 0.f,
							0.f, 0.f, 0.f, 1.f };

	float rotationZ[] = { cos(g_deltaTime), -sin(g_deltaTime), 0.f, 0.f,
							sin(g_deltaTime), cos(g_deltaTime), 0.f, 0.f,
							0.f, 0.f, 1.f, 0.f,
							0.f, 0.f, 0.f, 1.f };

	//Matrix::mul(rotationY, g_Matrices.m_WorldMatrix, g_Matrices.m_WorldMatrix);
	Matrix::mul(rotationZ, rotationY, g_Matrices.m_WorldMatrix);


	g_object.SetMatrixBuffer(g_Matrices);
	g_object.Render();

	g_SwapChain->Present(0, 0);
}

// ---

// Simple exemple illustratif afin de montrer comment recuperer les infos de la souris.
// Pour un controle plus pousse, voir les "Raw Inputs":
// 
struct MouseState
{
	int MouseWheel;
	short MousePosX, MousePosY;
	bool MouseDown[8];	// on pourrait gerer une souris 8 boutons dans l'ideal
} g_Mouse;

// Les messages Windows sont transmis a cette fonction de rappel (callback).
// Il faut retourner "1" ou "true" pour indiquer a Windows que l'on "consomme" le message
// autrement il faut appel la fonction de rappel par defaut de Windows (DefWindowProc)
// Plus de details ici: 
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_LBUTTONDOWN:
		g_Mouse.MouseDown[0] = true;
		return true;
	case WM_LBUTTONUP:
		g_Mouse.MouseDown[0] = false;
		return true;
	case WM_RBUTTONDOWN:
		g_Mouse.MouseDown[1] = true;
		return true;
	case WM_RBUTTONUP:
		g_Mouse.MouseDown[1] = false;
		return true;
	case WM_MOUSEWHEEL:
		g_Mouse.MouseWheel += GET_WHEEL_DELTA_WPARAM(wParam) > 0 ? +1 : -1;
		return true;
	case WM_MOUSEMOVE:
		g_Mouse.MousePosX = (short)(lParam);
		g_Mouse.MousePosY = (short)(lParam >> 16);
		return true;
	case WM_CHAR:
		if (wParam == VK_ESCAPE)
			PostQuitMessage(0);
		return true;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

// Fonction main() specifique a Windows. Le projet doit etre configure en projet Win32 (non console)
// et desactiver le support Unicode, autrement il faudrait utiliser la fonction wWinMain() et des WSTR / LPWSTR
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int)
{
	int exitCode = 0;
	static const char className[] = { "Isart" };
	static const char windowName[] = { "Fenetre et triangle de base" };
	// La WNDCLASS permet d'identifier la fenetre et determine comment celle-ci doit s'interfacer avec l'OS/Bureau
	WNDCLASS wc = { CS_CLASSDC, WndProc, 0, 0, GetModuleHandle(nullptr), nullptr, LoadCursor(nullptr, IDC_ARROW), nullptr, nullptr, className };
	RegisterClass(&wc);

	// Creation d'une fenetre en 720p centree sur le bureau
	const int app_width = 800;
	const int app_height = 800;
	DWORD style = WS_OVERLAPPEDWINDOW;
	RECT canvasRect = { 0, 0, app_width, app_height };
	// La fonction ci-dessous est necessaire afin que la fenetre soit un peu plus large
	// et garantir que le zone de dessin fait bien 720p dans notre cas.
	AdjustWindowRect(&canvasRect, style, FALSE);

	HWND hWnd = CreateWindow(className, windowName, style, (GetSystemMetrics(SM_CXSCREEN) - app_width) / 2, (GetSystemMetrics(SM_CYSCREEN) - app_height) / 2
							 , canvasRect.right - canvasRect.left, canvasRect.bottom - canvasRect.top, nullptr, nullptr, wc.hInstance, nullptr);

	if (hWnd == nullptr)
	{
		goto _quit;
		//
		// Oui j'aime les goto :)
		// Rarement utilise, le mot cle goto est pourtant bien pratique dans des cas comme celui-ci.
		// 
	}

	// Initialize Direct3D
	if (InitialiseDirect3DDevice(hWnd) < 0)
	{
		exitCode = 1;
		goto _cleanUp;
	}

	ShowWindow(hWnd, SW_SHOWDEFAULT);
	UpdateWindow(hWnd);

	// La boucle principale dite "message pump" sous Windows
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			continue;
		}

		// Apres avoir traite un ou plusieurs messages Windows on peut executer notre code de rendu
		Render();
	}

	AnimateWindow(hWnd, 200, AW_HIDE | AW_BLEND);

_cleanUp:
	ShutdownDirect3DDevice();
_quit:
	UnregisterClass(className, wc.hInstance);

	return exitCode;
}
