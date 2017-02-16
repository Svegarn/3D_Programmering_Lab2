#include <windowsx.h>
#include <chrono>
#include <sstream>

#include "GraphicsManager.h"
#include "Camera.h"


/*	TODO:

	* FIX CAMERA!! 
	* Deferred, change rendertargets of basic pass, finish shaders and change existing shaders
	* Shadow maping, continue (SRV is already created), have to create shaders etc
	* Normal mapping
	* Correct sampling
	* Anti-aliasing
	* FBX import
	* Tesselation w/ displacement mapping
	* Some culling technique
	* Skeletal animation
	* Animation blending
	* Node hierarchy, weapons n such

*/

#define	WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

float deltaTime = 0.0f;

HWND InitWindow(HINSTANCE hInstance);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
{
	CoInitialize(NULL);
	MSG msg = { 0 };
	HWND wndHandle = InitWindow(hInstance);

	if (wndHandle)
	{
		GraphicsManager* sGraphicsManager = &GraphicsManager::getInstance();
		Camera* sCamera = &Camera::getInstance();

		sGraphicsManager->initialize(wndHandle, WINDOW_WIDTH, WINDOW_HEIGHT);
		sCamera->Initialize(WINDOW_WIDTH, WINDOW_HEIGHT);
		sGraphicsManager->CreateCameraBuffer(sCamera->getView(), sCamera->getProjection(), sCamera->getPosition());

		ShowWindow(wndHandle, nCmdShow);

		auto frameTime = std::chrono::steady_clock::now();
		UINT framecount = 0;
		float elapsedTime = 0.0f;

		SetCursorPos(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);

		while (WM_QUIT != msg.message)
		{
			auto start = std::chrono::steady_clock::now();
			if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				sCamera->Update(deltaTime);
				sGraphicsManager->UpdateCameraBuffer(sCamera->getView(), sCamera->getPosition());
				sGraphicsManager->Update(deltaTime, elapsedTime);
				sGraphicsManager->Render();

				auto end = std::chrono::steady_clock::now();

				deltaTime = std::chrono::duration <float, std::milli>(end - start).count();
				elapsedTime = std::chrono::duration <float, std::milli>(end - frameTime).count();

				framecount++;
			}
		}

		DestroyWindow(wndHandle);
	}

	return (int) msg.wParam;
}

HWND InitWindow(HINSTANCE hInstance)
{
	WNDCLASSEX wcex = { 0 };
	wcex.cbSize = sizeof(WNDCLASSEX); 
	wcex.style          = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc    = WndProc;
	wcex.hInstance      = hInstance;
	wcex.lpszClassName = L"BTH_D3D_DEMO";
	if (!RegisterClassEx(&wcex))
		return false;

	RECT rc = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

	HWND handle = CreateWindow(
		L"BTH_D3D_DEMO",
		L"BTH Direct3D Demo",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		rc.right - rc.left,
		rc.bottom - rc.top,
		nullptr,
		nullptr,
		hInstance,
		nullptr);

	return handle;
}

LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch (message) 
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;		
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			PostQuitMessage(0);
			break;
		case VK_NUMPAD1:
			GraphicsManager::getInstance().CreateLab2Shaders();
			Sleep(100);
			break;
		}
	case WM_MOUSEMOVE:
		Camera::getInstance().MouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), deltaTime);
		break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}