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

	ID3D11Texture2D* mAlbedo_T2D = nullptr;
	ID3D11Texture2D* mNormal_T2D = nullptr;
	ID3D11Texture2D* mMaterial_T2D = nullptr;

	ID3D11ShaderResourceView* mAlbedo_SRV = nullptr;
	ID3D11ShaderResourceView* mNormal_SRV = nullptr;
	ID3D11ShaderResourceView* mMaterial_SRV = nullptr;

	ID3D11RenderTargetView* mAlbedo_RTV = nullptr;
	ID3D11RenderTargetView* mNormal_RTV = nullptr;
	ID3D11RenderTargetView* mMaterial_RTV = nullptr;

	// Resources
	ID3D11ShaderResourceView* mBTH_SRV = nullptr;

	// T2D
	ID3D11Texture2D* mDepthBuffer_T2D = nullptr;
	ID3D11Texture2D* mBTH_T2D = nullptr;

	ID3D11Buffer* mCameraCbuffer = nullptr;

	ID3D11SamplerState* mLinearClampSampler = nullptr;

	ID3D11InputLayout* mShadowVertexLayout = nullptr;
	ID3D11VertexShader* mShadowVertexShader = nullptr;

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
	void CreateGBuffers();
	void CreateShaderResources();
	void CreateSamplers();
	void SetViewport();
	void SetRasterizerState();
	void ResetRTV_SRV();

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

	HRESULT initialize(HWND wndHandle, UINT width, UINT height);
	void CreateShadowShaders();
	void CreateLab2Shaders();
	void CreateBasicShaders();
	void CreateDeferredShaders();
	void CreateCameraBuffer(DirectX::XMFLOAT4X4* view, DirectX::XMFLOAT4X4* projection, DirectX::XMFLOAT3* eyePos);
	void UpdateCameraBuffer(DirectX::XMFLOAT4X4* view, DirectX::XMFLOAT3* eyePos);

	ID3D11Device* getDevice();
	ID3D11DeviceContext* getDeviceContext();
	IDXGISwapChain* getSwapChain();
};

#endif