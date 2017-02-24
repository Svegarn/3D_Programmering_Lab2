#ifndef STATICMESH_H
#define STATICMESH_H

#include <fbxsdk.h>

#include "GraphicsManager.h"

using namespace fbxsdk;

class StaticMesh
{
private:
	ID3D11Buffer* mVertexBuffer = nullptr;
	ID3D11Buffer* mWorldMatrixBuffer = nullptr;
	DirectX::XMFLOAT4X4 mWorldMatrix;
	DataStructures::Material* mMaterial;

public:
	UINT vertexCount = 0;

	StaticMesh() {}
	StaticMesh(std::string filePath, DataStructures::Material* material, DirectX::XMMATRIX worldMatrix = DirectX::XMMatrixIdentity());
	StaticMesh(DataStructures::Vertex* vertexList, UINT vertexCount, DirectX::XMMATRIX worldMatrix = DirectX::XMMatrixIdentity());
	StaticMesh(std::vector<DataStructures::Vertex> &vertexList, DirectX::XMMATRIX worldMatrix = DirectX::XMMatrixIdentity());
	~StaticMesh();

	void Update();

	ID3D11Buffer** getVertexBuffer();
	ID3D11Buffer** getWorldMatrixBuffer();

	void setWorldMatrix(DirectX::XMMATRIX matrix);
	DirectX::XMFLOAT4X4* getWorldMatrix();

	void setMaterial(DataStructures::Material* material, std::wstring texturePath);
	DataStructures::Material* getMaterial();
};

#endif