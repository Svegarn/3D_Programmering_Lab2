#include "GraphicsManager.h"

void GraphicsManager::Update(float deltaTime, float elapsedTime) {
	UpdateCameraBuffer();

	this->deltaTime = deltaTime;
	this->elapsedTime = elapsedTime;

	if (GetAsyncKeyState(VK_NUMPAD1)) {
		GraphicsManager::getInstance().CreateLab2Shaders();
		Sleep(100);
	}
	if (GetAsyncKeyState(0x57)) // Forward
		GraphicsManager::getInstance().addToCamPos(0.0f, 0.0f, moveSpeed * deltaTime);
	if (GetAsyncKeyState(0x53)) // Backwards
		GraphicsManager::getInstance().addToCamPos(0.0f, 0.0f, -moveSpeed * deltaTime);
	if (GetAsyncKeyState(0x44)) // Strafe right
		GraphicsManager::getInstance().addToCamPos(moveSpeed * deltaTime);
	if (GetAsyncKeyState(0x41)) // Strafe left
		GraphicsManager::getInstance().addToCamPos(-moveSpeed * deltaTime);
	if (GetAsyncKeyState(VK_SPACE)) // Up
		GraphicsManager::getInstance().addToCamPos(0.0f, moveSpeed * deltaTime);
	if (GetAsyncKeyState(0x43)) // Down
		GraphicsManager::getInstance().addToCamPos(0.0f, -moveSpeed * deltaTime);

	// Cube rotation
	ContentManager::getInstance().getStaticMeshes()[0]->setWorldMatrix(
		DirectX::XMMatrixRotationAxis(DirectX::XMLoadFloat4(&DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f)), matAngle)
	);
	ContentManager::getInstance().getStaticMeshes()[0]->Update();
	matAngle += deltaTime * 0.0005f;
}

void GraphicsManager::Render()
{
	ResetRTV_SRV();

	mDeviceContext->ClearRenderTargetView(mBackbufferRTV, clearColor);
	mDeviceContext->ClearRenderTargetView(mAlbedo_RTV, clearColor);
	mDeviceContext->ClearRenderTargetView(mNormal_RTV, clearColor);
	mDeviceContext->ClearRenderTargetView(mMaterial_RTV, clearColor);

	mDeviceContext->ClearDepthStencilView(mDepthBuffer_DSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// ######################################################### SPINNING BOX

	mDeviceContext->OMSetRenderTargets(1, &mBackbufferRTV, mDepthBuffer_DSV);

	mDeviceContext->VSSetShader(mLab2VertexShader, nullptr, 0);
	mDeviceContext->HSSetShader(nullptr, nullptr, 0);
	mDeviceContext->DSSetShader(nullptr, nullptr, 0);
	mDeviceContext->GSSetShader(mLab2GeometryShader, nullptr, 0);
	mDeviceContext->PSSetShader(mLab2PixelShader, nullptr, 0);

	mDeviceContext->PSSetSamplers(0, 1, &mLinearClampSampler);

	mDeviceContext->GSSetConstantBuffers(0, 1, &mCameraCbuffer);
	mDeviceContext->GSSetConstantBuffers(1, 1, ContentManager::getInstance().getStaticMeshes()[0]->getWorldMatrixBuffer());
	mDeviceContext->PSSetConstantBuffers(0, 1, &mCameraCbuffer);

	mDeviceContext->PSSetShaderResources(0, 1, &mBTH_SRV);

	mDeviceContext->IASetVertexBuffers(0, 1, ContentManager::getInstance().getStaticMeshes()[0]->getVertexBuffer(), &vertexSize, &offset);

	mDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	mDeviceContext->IASetInputLayout(mLab2VertexLayout);

	mDeviceContext->Draw(6, 0);

	// ######################################################### GEOMETRY PASS

	ID3D11RenderTargetView* targets[3] = { mAlbedo_RTV, mNormal_RTV, mMaterial_RTV };

	mDeviceContext->OMSetRenderTargets(3, targets, mDepthBuffer_DSV);

	mDeviceContext->VSSetShader(mBasicVertexShader, nullptr, 0);
	mDeviceContext->GSSetShader(nullptr, nullptr, 0);
	mDeviceContext->PSSetShader(mBasicPixelShader, nullptr, 0);

	for (UINT i = 1; i < ContentManager::getInstance().getStaticMeshes().size(); i++)
	{
		mDeviceContext->PSSetShaderResources(0, 1, &ContentManager::getInstance().getStaticMeshes()[i]->getMaterial()->srv);

		mDeviceContext->VSSetConstantBuffers(0, 1, &mCameraCbuffer);
		mDeviceContext->VSSetConstantBuffers(1, 1, ContentManager::getInstance().getStaticMeshes()[i]->getWorldMatrixBuffer());

		mDeviceContext->IASetVertexBuffers(0, 1, ContentManager::getInstance().getStaticMeshes()[i]->getVertexBuffer(), &vertexSize, &offset);

		mDeviceContext->IASetInputLayout(mBasicVertexLayout);

		mDeviceContext->Draw(ContentManager::getInstance().getStaticMeshes()[i]->vertexCount, 0);
	}

	// #########################################################

	mDeviceContext->OMSetRenderTargets(1, &mBackbufferRTV, NULL);

	mDeviceContext->VSSetShader(mDeferredVertexShader, nullptr, 0);
	mDeviceContext->GSSetShader(nullptr, nullptr, 0);
	mDeviceContext->PSSetShader(mDeferredPixelShader, nullptr, 0);

	ID3D11ShaderResourceView* sources[4] = { mShadow_SRV, mAlbedo_SRV, mNormal_SRV, mMaterial_SRV };

	mDeviceContext->PSSetShaderResources(0, 4, sources);

	mDeviceContext->IASetInputLayout(NULL);
	mDeviceContext->IASetVertexBuffers(0, 0, NULL, 0, 0);
	mDeviceContext->IASetIndexBuffer(NULL, (DXGI_FORMAT)0, 0);

	mDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	mDeviceContext->Draw(3, 0);

	// #########################################################

	mSwapChain->Present(0, 0);
}

HRESULT GraphicsManager::CreateDirect3DContext(HWND wndHandle, UINT width, UINT height)
{
	mWindowWidth = width;
	mWindowHeight = height;

	// create a struct to hold information about the swap chain
	DXGI_SWAP_CHAIN_DESC scd;

	// clear out the struct for use
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

	// fill the swap chain description struct
	scd.BufferCount = 1;                                    // one back buffer
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     // use 32-bit color
	scd.BufferDesc.Width = mWindowWidth;
	scd.BufferDesc.Height = mWindowHeight;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;      // how swap chain is to be used
	scd.OutputWindow = wndHandle;                           // the window to be used
	scd.SampleDesc.Count = 1;                               // how many multisamples
	scd.Windowed = TRUE;                                    // windowed/full-screen mode

															// create a device, device context and swap chain using the information in the scd struct
	HRESULT hr = D3D11CreateDeviceAndSwapChain(NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		D3D11_CREATE_DEVICE_DEBUG,
		NULL,
		NULL,
		D3D11_SDK_VERSION,
		&scd,
		&mSwapChain,
		&mDevice,
		NULL,
		&mDeviceContext);

	if (SUCCEEDED(hr))
	{
		// get the address of the back buffer
		ID3D11Texture2D* pBackBuffer = nullptr;
		mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

		// use the back buffer address to create the render target
		mDevice->CreateRenderTargetView(pBackBuffer, NULL, &mBackbufferRTV);
		pBackBuffer->Release();
	}

	SetViewport();
	CreateShadowShaders();
	CreateLab2Shaders();
	CreateBasicShaders();
	CreateDeferredShaders();
	CreateCameraBuffer();
	SetRasterizerState();
	CreateDepthBuffer();
	CreateGBuffers();
	CreateShaderResources();
	CreateSamplers();

	ContentManager::getInstance().initializeContent();

	return hr;
}

void GraphicsManager::CreateShadowShaders() {
	//create vertex shader
	if (mShadowVertexShader != nullptr)
		mShadowVertexShader->Release();

	ID3DBlob* pVS = nullptr;
	ID3DBlob* errorblob = nullptr;
	HRESULT hr = D3DCompileFromFile(
		L"Shaders/Shadow/Shadow_VS.hlsl", // filename
		nullptr,		// optional macros
		nullptr,		// optional include files
		"VS_main",		// entry point
		"vs_5_0",		// shader model (target)
		0,				// shader compile options			// here DEBUGGING OPTIONS
		0,				// effect compile options
		&pVS,			// double pointer to ID3DBlob		
		&errorblob			// pointer for Error Blob messages.
							// how to use the Error blob, see here
							// https://msdn.microsoft.com/en-us/library/windows/desktop/hh968107(v=vs.85).aspx
		);

	if (errorblob) {
		OutputDebugStringA((char*)errorblob->GetBufferPointer());
		errorblob->Release();
	}

	hr = mDevice->CreateVertexShader(pVS->GetBufferPointer(), pVS->GetBufferSize(), nullptr, &mShadowVertexShader);

	if (mShadowVertexLayout == nullptr) {
		//create input layout (verified using vertex shader)
		D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		mDevice->CreateInputLayout(inputDesc, ARRAYSIZE(inputDesc), pVS->GetBufferPointer(), pVS->GetBufferSize(), &mShadowVertexLayout);
		// we do not need anymore this COM object, so we release it.
	}
	pVS->Release();
}

void GraphicsManager::CreateLab2Shaders()
{
	//create vertex shader
	if (mLab2VertexShader != nullptr)
		mLab2VertexShader->Release();

	ID3DBlob* pVS = nullptr;
	ID3DBlob* errorblob = nullptr;
	HRESULT hr = D3DCompileFromFile(
		L"Shaders/Lab2Shaders/Lab2_VS.hlsl", // filename
		nullptr,		// optional macros
		nullptr,		// optional include files
		"VS_main",		// entry point
		"vs_5_0",		// shader model (target)
		0,				// shader compile options			// here DEBUGGING OPTIONS
		0,				// effect compile options
		&pVS,			// double pointer to ID3DBlob		
		&errorblob			// pointer for Error Blob messages.
							// how to use the Error blob, see here
							// https://msdn.microsoft.com/en-us/library/windows/desktop/hh968107(v=vs.85).aspx
		);

	if (errorblob) {
		OutputDebugStringA((char*)errorblob->GetBufferPointer());
		errorblob->Release();
	}

	hr = mDevice->CreateVertexShader(pVS->GetBufferPointer(), pVS->GetBufferSize(), nullptr, &mLab2VertexShader);

	if (mLab2VertexLayout == nullptr) {
		//create input layout (verified using vertex shader)
		D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		mDevice->CreateInputLayout(inputDesc, ARRAYSIZE(inputDesc), pVS->GetBufferPointer(), pVS->GetBufferSize(), &mLab2VertexLayout);
		// we do not need anymore this COM object, so we release it.
	}
	pVS->Release();

	//create geometry shader
	if (mLab2GeometryShader != nullptr)
		mLab2GeometryShader->Release();

	ID3DBlob* pGS = nullptr;
	D3DCompileFromFile(
		L"Shaders/Lab2Shaders/Lab2_GS.hlsl",
		nullptr,
		nullptr,
		"GS_main",
		"gs_5_0",
		0,
		0,
		&pGS,
		nullptr
		);

	mDevice->CreateGeometryShader(pGS->GetBufferPointer(), pGS->GetBufferSize(), nullptr, &mLab2GeometryShader);
	pGS->Release();

	//create pixel shader
	if (mLab2PixelShader != nullptr)
		mLab2PixelShader->Release();

	ID3DBlob* pPS = nullptr;
	D3DCompileFromFile(
		L"Shaders/Lab2Shaders/Lab2_PS.hlsl", // filename
		nullptr,		// optional macros
		nullptr,		// optional include files
		"PS_main",		// entry point
		"ps_5_0",		// shader model (target)
		0,				// shader compile options
		0,				// effect compile options
		&pPS,			// double pointer to ID3DBlob		
		nullptr			// pointer for Error Blob messages.
						// how to use the Error blob, see here
						// https://msdn.microsoft.com/en-us/library/windows/desktop/hh968107(v=vs.85).aspx
		);

	mDevice->CreatePixelShader(pPS->GetBufferPointer(), pPS->GetBufferSize(), nullptr, &mLab2PixelShader);
	// we do not need anymore this COM object, so we release it.
	pPS->Release();
}

void GraphicsManager::CreateBasicShaders()
{
	//create vertex shader
	if (mBasicVertexShader != nullptr)
		mBasicVertexShader->Release();

	ID3DBlob* pVS = nullptr;
	ID3DBlob* errorblob = nullptr;
	HRESULT hr = D3DCompileFromFile(
		L"Shaders/BasicShaders/Basic_VS.hlsl", // filename
		nullptr,		// optional macros
		nullptr,		// optional include files
		"VS_main",		// entry point
		"vs_5_0",		// shader model (target)
		0,				// shader compile options			// here DEBUGGING OPTIONS
		0,				// effect compile options
		&pVS,			// double pointer to ID3DBlob		
		&errorblob			// pointer for Error Blob messages.
							// how to use the Error blob, see here
							// https://msdn.microsoft.com/en-us/library/windows/desktop/hh968107(v=vs.85).aspx
		);

	if (errorblob) {
		OutputDebugStringA((char*)errorblob->GetBufferPointer());
		errorblob->Release();
	}

	hr = mDevice->CreateVertexShader(pVS->GetBufferPointer(), pVS->GetBufferSize(), nullptr, &mBasicVertexShader);

	if (mBasicVertexLayout == nullptr) {
		//create input layout (verified using vertex shader)
		D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		mDevice->CreateInputLayout(inputDesc, ARRAYSIZE(inputDesc), pVS->GetBufferPointer(), pVS->GetBufferSize(), &mBasicVertexLayout);
		// we do not need anymore this COM object, so we release it.
	}
	pVS->Release();

	//create pixel shader
	if (mBasicPixelShader != nullptr)
		mBasicPixelShader->Release();

	ID3DBlob* pPS = nullptr;
	D3DCompileFromFile(
		L"Shaders/BasicShaders/Basic_PS.hlsl", // filename
		nullptr,		// optional macros
		nullptr,		// optional include files
		"PS_main",		// entry point
		"ps_5_0",		// shader model (target)
		0,				// shader compile options
		0,				// effect compile options
		&pPS,			// double pointer to ID3DBlob		
		nullptr			// pointer for Error Blob messages.
						// how to use the Error blob, see here
						// https://msdn.microsoft.com/en-us/library/windows/desktop/hh968107(v=vs.85).aspx
		);

	mDevice->CreatePixelShader(pPS->GetBufferPointer(), pPS->GetBufferSize(), nullptr, &mBasicPixelShader);
	// we do not need anymore this COM object, so we release it.
	pPS->Release();
}

void GraphicsManager::CreateDeferredShaders()
{
	//create vertex shader
	if (mDeferredVertexShader != nullptr)
		mDeferredVertexShader->Release();

	ID3DBlob* pVS = nullptr;
	ID3DBlob* errorblob = nullptr;
	HRESULT hr = D3DCompileFromFile(
		L"Shaders/DeferredShaders/Deferred_VS.hlsl", // filename
		nullptr,		// optional macros
		nullptr,		// optional include files
		"VS_main",		// entry point
		"vs_5_0",		// shader model (target)
		0,				// shader compile options			// here DEBUGGING OPTIONS
		0,				// effect compile options
		&pVS,			// double pointer to ID3DBlob		
		&errorblob			// pointer for Error Blob messages.
							// how to use the Error blob, see here
							// https://msdn.microsoft.com/en-us/library/windows/desktop/hh968107(v=vs.85).aspx
		);

	if (errorblob) {
		OutputDebugStringA((char*)errorblob->GetBufferPointer());
		errorblob->Release();
	}

	hr = mDevice->CreateVertexShader(pVS->GetBufferPointer(), pVS->GetBufferSize(), nullptr, &mDeferredVertexShader);

	if (mDeferredVertexLayout == nullptr) {
		//create input layout (verified using vertex shader)
		D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		mDevice->CreateInputLayout(inputDesc, ARRAYSIZE(inputDesc), pVS->GetBufferPointer(), pVS->GetBufferSize(), &mDeferredVertexLayout);
		// we do not need anymore this COM object, so we release it.
	}
	pVS->Release();

	//create pixel shader
	if (mDeferredPixelShader != nullptr)
		mDeferredPixelShader->Release();

	ID3DBlob* pPS = nullptr;
	D3DCompileFromFile(
		L"Shaders/DeferredShaders/Deferred_PS.hlsl", // filename
		nullptr,		// optional macros
		nullptr,		// optional include files
		"PS_main",		// entry point
		"ps_5_0",		// shader model (target)
		0,				// shader compile options
		0,				// effect compile options
		&pPS,			// double pointer to ID3DBlob		
		nullptr			// pointer for Error Blob messages.
						// how to use the Error blob, see here
						// https://msdn.microsoft.com/en-us/library/windows/desktop/hh968107(v=vs.85).aspx
		);

	mDevice->CreatePixelShader(pPS->GetBufferPointer(), pPS->GetBufferSize(), nullptr, &mDeferredPixelShader);
	// we do not need anymore this COM object, so we release it.
	pPS->Release();
}

void GraphicsManager::CreateDepthBuffer()
{
	D3D11_TEXTURE2D_DESC depthMapDesc;

	ZeroMemory(&depthMapDesc, sizeof(depthMapDesc));

	depthMapDesc.Width = mWindowWidth;
	depthMapDesc.Height = mWindowHeight;
	depthMapDesc.MipLevels = 1;
	depthMapDesc.ArraySize = 1;
	depthMapDesc.Format = DXGI_FORMAT_R32_TYPELESS;;
	depthMapDesc.SampleDesc.Count = 1;
	depthMapDesc.SampleDesc.Quality = 0;
	depthMapDesc.Usage = D3D11_USAGE_DEFAULT;
	depthMapDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	depthMapDesc.CPUAccessFlags = 0;
	depthMapDesc.MiscFlags = 0;

	hr = mDevice->CreateTexture2D(&depthMapDesc, nullptr, &mDepthBuffer_T2D);

	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	descDSV.Format = DXGI_FORMAT_D32_FLOAT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	descDSV.Flags = 0;

	hr = mDevice->CreateDepthStencilView(mDepthBuffer_T2D, &descDSV, &mDepthBuffer_DSV);

	D3D11_SHADER_RESOURCE_VIEW_DESC shadowDesc;
	shadowDesc.Format = DXGI_FORMAT_R32_FLOAT;
	shadowDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shadowDesc.Texture2D.MostDetailedMip = 0;
	shadowDesc.Texture2D.MipLevels = 1;

	hr = mDevice->CreateShaderResourceView(mDepthBuffer_T2D, &shadowDesc, &mShadow_SRV);
}

void GraphicsManager::CreateGBuffers() {
	D3D11_TEXTURE2D_DESC t2dDesc;
	t2dDesc.Width = mWindowWidth;
	t2dDesc.Height = mWindowHeight;
	t2dDesc.MipLevels = t2dDesc.ArraySize = 1;
	t2dDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	t2dDesc.SampleDesc.Count = 1;
	t2dDesc.SampleDesc.Quality = 0;
	t2dDesc.Usage = D3D11_USAGE_DEFAULT;
	t2dDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	t2dDesc.CPUAccessFlags = 0;
	t2dDesc.MiscFlags = 0;

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = t2dDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;

	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	rtvDesc.Format = t2dDesc.Format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;

	HRESULT hr = mDevice->CreateTexture2D(&t2dDesc, NULL, &mAlbedo_T2D);
	hr = mDevice->CreateTexture2D(&t2dDesc, NULL, &mNormal_T2D);
	hr = mDevice->CreateTexture2D(&t2dDesc, NULL, &mMaterial_T2D);

	hr = mDevice->CreateShaderResourceView(mAlbedo_T2D, &srvDesc, &mAlbedo_SRV);
	hr = mDevice->CreateShaderResourceView(mNormal_T2D, &srvDesc, &mNormal_SRV);
	hr = mDevice->CreateShaderResourceView(mMaterial_T2D, &srvDesc, &mMaterial_SRV);

	hr = mDevice->CreateRenderTargetView(mAlbedo_T2D, &rtvDesc, &mAlbedo_RTV);
	hr = mDevice->CreateRenderTargetView(mNormal_T2D, &rtvDesc, &mNormal_RTV);
	hr = mDevice->CreateRenderTargetView(mMaterial_T2D, &rtvDesc, &mMaterial_RTV);
}

void GraphicsManager::CreateShaderResources()
{
	D3D11_TEXTURE2D_DESC desc;
	desc.Width = 64;
	desc.Height = 64;
	desc.MipLevels = desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory(&data, sizeof(data));
	data.pSysMem = (void*)BTH_IMAGE_DATA;
	data.SysMemPitch = 64 * 4 * sizeof(char);

	mDevice->CreateTexture2D(&desc, &data, &mBTH_T2D);

	mDevice->CreateShaderResourceView(mBTH_T2D, NULL, &mBTH_SRV);
}

void GraphicsManager::CreateSamplers()
{
	D3D11_SAMPLER_DESC desc;
	desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.MinLOD = 0.0f;
	desc.MaxLOD = 0.0f;
	desc.MaxAnisotropy = 1;
	desc.ComparisonFunc = D3D11_COMPARISON_NEVER;

	HRESULT hr = mDevice->CreateSamplerState(&desc, &mLinearClampSampler);
}

void GraphicsManager::SetViewport()
{
	D3D11_VIEWPORT vp;
	vp.Width = (float)mWindowWidth;
	vp.Height = (float)mWindowHeight;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	mDeviceContext->RSSetViewports(1, &vp);
}

void GraphicsManager::SetRasterizerState()
{
	D3D11_RASTERIZER_DESC rasterizerState;
	rasterizerState.FillMode = D3D11_FILL_SOLID;
	rasterizerState.CullMode = D3D11_CULL_NONE;
	rasterizerState.FrontCounterClockwise = false;
	rasterizerState.DepthBias = 0;
	rasterizerState.DepthBiasClamp = 0;
	rasterizerState.SlopeScaledDepthBias = 0;
	rasterizerState.DepthClipEnable = true;
	rasterizerState.ScissorEnable = false;
	rasterizerState.MultisampleEnable = false;
	rasterizerState.AntialiasedLineEnable = false;

	mDevice->CreateRasterizerState(&rasterizerState, &mRasterState);
	mDeviceContext->RSSetState(mRasterState);
}

void GraphicsManager::ResetRTV_SRV() {
	//tempDSV = NULL;
	//int x = 0;
	//
	//for (int i = 0; i < 4; i++) {
	//	tempRTV[i] = NULL;
	//	for (int y = 0; y < 2; y++) {
	//		tempSRV[y + x] = NULL;
	//	}
	//	x += 2;
	//}

	mDeviceContext->OMSetRenderTargets(4, tempRTV, tempDSV);
	mDeviceContext->PSSetShaderResources(0, 8, tempSRV);
}

ID3D11Device* GraphicsManager::getDevice() {
	return mDevice;
}

ID3D11DeviceContext* GraphicsManager::getDeviceContext() {
	return mDeviceContext;
}

IDXGISwapChain* GraphicsManager::getSwapChain() {
	return mSwapChain;
}

void GraphicsManager::CreateCameraBuffer()
{
	// View
	XMStoreFloat4x4(
		&mCameraData.view,
		DirectX::XMMatrixTranspose(DirectX::XMMatrixLookAtLH(
			DirectX::XMLoadFloat4(&mEyePos),
			DirectX::XMLoadFloat4(&mLookAt),
			DirectX::XMLoadFloat4(&DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f))
			)
			));

	// Projection
	XMStoreFloat4x4(
		&mCameraData.projection,
		DirectX::XMMatrixTranspose(DirectX::XMMatrixPerspectiveFovLH(
			DirectX::XM_PI * 0.45f,
			(float)mWindowWidth / (float)mWindowHeight,
			0.1f,
			200.0f
			)
			));

	// DT
	mCameraData.DT = { 0.0f, 0.0f, 0.0f, 0.0f };
	// Eye position
	mCameraData.eyePos = mEyePos;

	D3D11_BUFFER_DESC desc;
	desc.ByteWidth = sizeof(DataStructures::CameraCbuffer);
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = &mCameraData;
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	HRESULT hr = mDevice->CreateBuffer(&desc, &data, &mCameraCbuffer);
}

void GraphicsManager::UpdateCameraBuffer()
{
	//View
	XMStoreFloat4x4(
		&mCameraData.view,
		XMMatrixTranspose(DirectX::XMMatrixLookAtLH(
			XMLoadFloat4(&mEyePos),
			XMLoadFloat4(&mLookAt),
			XMLoadFloat4(&DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f))
			)
			));

	// DT
	mCameraData.DT = { deltaTime, elapsedTime, 0.0f, 0.0f };
	mCameraData.eyePos = mEyePos;

	D3D11_MAPPED_SUBRESOURCE subresource;
	ZeroMemory(&subresource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	mDeviceContext->Map(mCameraCbuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subresource);
	memcpy((DataStructures::CameraCbuffer*)subresource.pData, &mCameraData, sizeof(DataStructures::CameraCbuffer));
	mDeviceContext->Unmap(mCameraCbuffer, 0);
}

void GraphicsManager::addToCamPos(float x, float y, float z)
{
	mEyePos.x += x;
	mEyePos.y += y;
	mEyePos.z += z;

	mLookAt.x += x;
	mLookAt.y += y;
	mLookAt.z += z;
}

GraphicsManager::~GraphicsManager()
{
	mDepthBuffer_T2D->Release();
	mBTH_T2D->Release();

	mDepthBuffer_DSV->Release();

	mShadow_SRV->Release();
	mBTH_SRV->Release();

	mAlbedo_SRV->Release();
	mNormal_SRV->Release();
	mMaterial_SRV->Release();

	mAlbedo_RTV->Release();
	mNormal_RTV->Release();
	mMaterial_RTV->Release();

	mAlbedo_T2D->Release();
	mNormal_T2D->Release();
	mMaterial_T2D->Release();

	mCameraCbuffer->Release();
	mLinearClampSampler->Release();

	mShadowVertexLayout->Release();
	mShadowVertexShader->Release();

	mLab2VertexLayout->Release();
	mLab2VertexShader->Release();
	mLab2GeometryShader->Release();
	mLab2PixelShader->Release();

	mBasicVertexLayout->Release();
	mBasicVertexShader->Release();
	mBasicPixelShader->Release();

	mDeferredVertexLayout->Release();
	mDeferredVertexShader->Release();
	mDeferredPixelShader->Release();

	mBackbufferRTV->Release();
	mRasterState->Release();
	mSwapChain->Release();
	mDevice->Release();
	mDeviceContext->Release();
}