#include "ContentManager.h"

ContentManager::ContentManager() {

}

void ContentManager::initializeContent()
{
	// lambert1
	DataStructures::Material* lambert1 = new DataStructures::Material;
	lambert1->data.specular.w = -1.0f; // Lambert
	lambert1->data.color = { 0.9f, 0.9f, 0.9f, 1.0f };
	
	SetCbuffer(lambert1);
	materials.push_back(lambert1);

	// phong1
	DataStructures::Material* phong1 = new DataStructures::Material;
	phong1->data.specular.w = 20.0f;
	phong1->data.misc.x = 0.00003f;
	phong1->data.hasTexture.x = 1;

	SetTexture(phong1, L"Assets/code.dds");
	SetCbuffer(phong1);
	materials.push_back(phong1);

	//// Cube
	DataStructures::Vertex vertices[6] = {
		DirectX::XMFLOAT3(-0.5f, -0.5f, 0.0f),
		DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f),
		DirectX::XMFLOAT2(0.0f, 1.0f),
	
		DirectX::XMFLOAT3(-0.5f, 0.5f, 0.0f),
		DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f),
		DirectX::XMFLOAT2(0.0f, 0.0f),
	
		DirectX::XMFLOAT3(0.5f, 0.5f, 0.0f),
		DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f),
		DirectX::XMFLOAT2(1.0f, 0.0f),
	
		DirectX::XMFLOAT3(-0.5f, -0.5f, 0.0f),
		DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f),
		DirectX::XMFLOAT2(0.0f, 1.0f),
	
		DirectX::XMFLOAT3(0.5f, 0.5f, 0.0f),
		DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f),
		DirectX::XMFLOAT2(1.0f, 0.0f),
	
		DirectX::XMFLOAT3(0.5f, -0.5f, 0.0f),
		DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f),
		DirectX::XMFLOAT2(1.0f, 1.0f)
	};
	
	StaticMesh* cube = new StaticMesh(vertices, 6);
	staticMeshes.push_back(cube);

	// Terrain
	StaticMesh* terrain = new StaticMesh(
		"Assets/terrain.fbx",
		materials[DataStructures::Materials::MATERIAL_LAMBERT1],
		DirectX::XMMatrixScaling(1000.0f, 1000.0f, 1000.0f));

	staticMeshes.push_back(terrain);

	// Tree
	StaticMesh* tree = new StaticMesh(
		"Assets/tree.fbx",
		materials[DataStructures::Materials::MATERIAL_PHONG1],
		DirectX::XMMatrixScaling(100.0f, 100.0f, 100.0f)
		);

	staticMeshes.push_back(tree);
}

ContentManager::~ContentManager()
{
	for (UINT i = 0; i < staticMeshes.size(); i++)
		delete staticMeshes[i];

	for (UINT i = 0; i < materials.size(); i++)
	{
		if (materials[i]->srv != nullptr)
			materials[i]->srv->Release();

		if (materials[i]->cBuffer != nullptr)
			materials[i]->cBuffer->Release();

		delete materials[i];
	}
}

std::vector<StaticMesh*>& ContentManager::getStaticMeshes() {
	return staticMeshes;
}


void ContentManager::SetTexture(DataStructures::Material* material, std::wstring texturePath) {
	HRESULT hr = DirectX::CreateDDSTextureFromFile(
		GraphicsManager::getInstance().getDevice(),
		texturePath.c_str(),
		NULL,
		&material->srv
		);

	if (!SUCCEEDED(hr))
	{
		OutputDebugString(L"Could not load material texture!");
		exit(-1);
	}
}
void ContentManager::SetCbuffer(DataStructures::Material* material) {
	D3D11_BUFFER_DESC desc;
	desc.ByteWidth = sizeof(DataStructures::MaterialData);
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA subData;
	subData.pSysMem = &material->data;
	subData.SysMemPitch = 0;
	subData.SysMemSlicePitch = 0;

	HRESULT hr = GraphicsManager::getInstance().getDevice()->CreateBuffer(&desc, &subData, &material->cBuffer);
}