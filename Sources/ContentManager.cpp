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

	// Terrain
	StaticMesh* terrain = new StaticMesh("Assets/terrain.fbx");

	terrain->setWorldMatrix(DirectX::XMMatrixTranslation(0.0f, 0.0f, 0.0f));

	terrain->setMaterial(
		DataStructures::LambertData(),
		L"Assets/code.dds"
		);

	staticMeshes.push_back(terrain);
}

ContentManager::~ContentManager()
{
	for (UINT i = 0; i < staticMeshes.size(); i++)
		delete staticMeshes[i];
}

std::vector<StaticMesh*>& ContentManager::getStaticMeshes() {
	return staticMeshes;
}