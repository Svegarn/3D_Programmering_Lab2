#include <windows.h>
#include <chrono>

#include "GraphicsManager.h"


/*	TODO:

	* Deferred, add function to create G-Buffers, finish shaders and change existing shaders
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

HWND InitWindow(HINSTANCE hInstance);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
{
	CoInitialize(NULL);
	MSG msg = { 0 };
	HWND wndHandle = InitWindow(hInstance);

	if (wndHandle)
	{
		GraphicsManager::getInstance().CreateDirect3DContext(wndHandle, WINDOW_WIDTH, WINDOW_HEIGHT);
		
		ShowWindow(wndHandle, nCmdShow);

		auto frameTime = std::chrono::steady_clock::now();
		UINT framecount = 0;
		float deltaTime = 0.0f;
		float elapsedTime = 0.0f;

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
				if (GetAsyncKeyState(VK_ESCAPE)) // Quit
					msg.message = WM_QUIT;

				//SetCursorPos(0, 0);
				GraphicsManager::getInstance().Update(deltaTime, elapsedTime);
				GraphicsManager::getInstance().Render();

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
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}