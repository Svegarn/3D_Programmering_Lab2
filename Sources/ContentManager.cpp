#include "ContentManager.h"

ContentManager::ContentManager() {

}

void ContentManager::initializeContent()
{
	// Cube
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

	// Floor
	DataStructures::Vertex floorVertices[6] = {
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

	StaticMesh* floor = new StaticMesh(
		floorVertices,
		6,
		DirectX::XMMatrixMultiply(
			DirectX::XMMatrixMultiply(
				DirectX::XMMatrixRotationAxis(DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), DirectX::XMConvertToRadians(90.0f)),
				DirectX::XMMatrixScaling(40.0f, 1.0f, 40.0f)),
			DirectX::XMMatrixTranslation(0.0f, -2.0f, 0.0f)));

	floor->setMaterial(
		DataStructures::LambertData(),
		L"Assets/code.dds"
		);

	staticMeshes.push_back(floor);

	StaticMesh* sphere = new StaticMesh("Assets/terrain.fbx");
	staticMeshes.push_back(sphere);
}

ContentManager::~ContentManager()
{
	for (UINT i = 0; i < staticMeshes.size(); i++)
		delete staticMeshes[i];
}

std::vector<StaticMesh*>& ContentManager::getStaticMeshes() {
	return staticMeshes;
}