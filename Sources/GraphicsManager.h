#ifndef GRAPHICSMANAGER_H
#define GRAPHICSMANAGER_H

#include <d3d11.h>
#include <d3dcompiler.h>

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")

#include "../Resources/bth_image.h"
#include "../Resources/DDSTextureLoader/DDSTextureLoader.h"
#include "DataStructures.h"
#include "ContentManager.h"

class GraphicsManager
{
private:
	GraphicsManager() {}
	~GraphicsManager();

	HRESULT hr;

	UINT mWindowWidth = 0;
	UINT mWindowHeight = 0;

	float deltaTime = 0.0f;
	float elapsedTime = 0.0f;

	DataStructures::CameraCbuffer mCameraData;
	float matAngle = 0.0f;
	float moveSpeed = 0.01f;
	DirectX::XMFLOAT4 mEyePos = { 0.0f, 0.0f, -2.0f, 1.0f };
	DirectX::XMFLOAT4 mLookAt = { 0.0f, 0.0f, 0.0f, 1.0f };

	float clearColor[4] = { 0.2f, 0.2f, 0.2f, 1.0f };
	UINT32 vertexSize = sizeof(DataStructures::Vertex);
	UINT32 offset = 0;

	IDXGISwapChain* mSwapChain = nullptr;
	ID3D11RasterizerState * mRasterState = nullptr;
	ID3D11Device* mDevice = nullptr;
	ID3D11DeviceContext* mDeviceContext = nullptr;
	ID3D11RenderTargetView* mBackbufferRTV = nullptr;
	ID3D11DepthStencilView* mDepthBuffer_DSV = nullptr;

	// G-Buffers
	ID3D11ShaderResourceView* mShadow_SRV = nullptr;

	// Resources
	ID3D11ShaderResourceView* mBTH_SRV = nullptr;

	// T2D
	ID3D11Texture2D* mDepthBuffer_T2D = nullptr;
	ID3D11Texture2D* mBTH_T2D = nullptr;

	ID3D11Buffer* mCameraCbuffer = nullptr;

	ID3D11SamplerState* mLinearClampSampler = nullptr;

	ID3D11InputLayout* mLab2VertexLayout = nullptr;
	ID3D11VertexShader* mLab2VertexShader = nullptr;
	ID3D11GeometryShader* mLab2GeometryShader = nullptr;
	ID3D11PixelShader* mLab2PixelShader = nullptr;

	ID3D11InputLayout* mBasicVertexLayout = nullptr;
	ID3D11VertexShader* mBasicVertexShader = nullptr;
	ID3D11PixelShader* mBasicPixelShader = nullptr;

	ID3D11InputLayout* mDeferredVertexLayout = nullptr;
	ID3D11VertexShader* mDeferredVertexShader = nullptr;
	ID3D11PixelShader* mDeferredPixelShader = nullptr;

	ID3D11DepthStencilView* tempDSV = NULL;
	ID3D11RenderTargetView* tempRTV[4] = { NULL };
	ID3D11ShaderResourceView* tempSRV[8] = { NULL };

	void CreateDepthBuffer();
	void CreateShaderResources();
	void CreateSamplers();
	void SetViewport();
	void SetRasterizerState();
	void ResetRTV_SRV();
	void CreateCameraBuffer();
	void UpdateCameraBuffer();

public:
	static GraphicsManager& getInstance()
	{
		static GraphicsManager instance;
		return instance;
	}

	GraphicsManager(GraphicsManager const&) = delete;
	void operator=(GraphicsManager const&) = delete;

	void Update(float deltaTime, float elapsedTime);
	void Render();

	HRESULT CreateDirect3DContext(HWND wndHandle, UINT width, UINT height);
	void CreateLab2Shaders();
	void CreateBasicShaders();
	void CreateDeferredShaders();

	ID3D11Device* getDevice();
	ID3D11DeviceContext* getDeviceContext();
	IDXGISwapChain* getSwapChain();

	void addToCamPos(float x = 0.0f, float y = 0.0f, float z = 0.0f);
};

#endif