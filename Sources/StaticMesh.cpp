#include "StaticMesh.h"

StaticMesh::StaticMesh(DataStructures::Vertex* vertexList, UINT vertexCount, DirectX::XMMATRIX worldMatrix)
{
	DirectX::XMStoreFloat4x4(&mWorldMatrix, DirectX::XMMatrixTranspose(worldMatrix));

	// World matrix cbuffer
	D3D11_BUFFER_DESC matrixDesc;
	matrixDesc.ByteWidth = sizeof(DirectX::XMFLOAT4X4);
	matrixDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixDesc.MiscFlags = 0;
	matrixDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA matrixData;
	matrixData.pSysMem = &mWorldMatrix;
	matrixData.SysMemPitch = 0;
	matrixData.SysMemSlicePitch = 0;

	GraphicsManager::getInstance().getDevice()->CreateBuffer(&matrixDesc, &matrixData, &mWorldMatrixBuffer);

	// Vertex buffer
	D3D11_BUFFER_DESC vertexDesc;
	memset(&vertexDesc, 0, sizeof(vertexDesc));
	vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexDesc.ByteWidth = vertexCount * sizeof(DataStructures::Vertex);

	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = vertexList;
	GraphicsManager::getInstance().getDevice()->CreateBuffer(&vertexDesc, &vertexData, &mVertexBuffer);
}

StaticMesh::StaticMesh(std::vector<DataStructures::Vertex> &vertexList, DirectX::XMMATRIX worldMatrix)
{
	DirectX::XMStoreFloat4x4(&mWorldMatrix, DirectX::XMMatrixTranspose(worldMatrix));

	// World matrix cbuffer
	D3D11_BUFFER_DESC matrixDesc;
	matrixDesc.ByteWidth = sizeof(DirectX::XMFLOAT4X4);
	matrixDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixDesc.MiscFlags = 0;
	matrixDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA matrixData;
	matrixData.pSysMem = &mWorldMatrix;
	matrixData.SysMemPitch = 0;
	matrixData.SysMemSlicePitch = 0;

	GraphicsManager::getInstance().getDevice()->CreateBuffer(&matrixDesc, &matrixData, &mWorldMatrixBuffer);

	// Vertex buffer
	D3D11_BUFFER_DESC vertexDesc;
	memset(&vertexDesc, 0, sizeof(vertexDesc));
	vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexDesc.ByteWidth = vertexList.size() * sizeof(DataStructures::Vertex);

	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = vertexList.data();
	GraphicsManager::getInstance().getDevice()->CreateBuffer(&vertexDesc, &vertexData, &mVertexBuffer);
}

StaticMesh::~StaticMesh()
{
	mVertexBuffer->Release();
	mWorldMatrixBuffer->Release();
}

void StaticMesh::Update()
{
	D3D11_MAPPED_SUBRESOURCE subresource;
	ZeroMemory(&subresource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	GraphicsManager::getInstance().getDeviceContext()->Map(mWorldMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subresource);
	memcpy((DirectX::XMFLOAT4X4*)subresource.pData, &mWorldMatrix, sizeof(DirectX::XMFLOAT4X4));
	GraphicsManager::getInstance().getDeviceContext()->Unmap(mWorldMatrixBuffer, 0);
}

ID3D11Buffer** StaticMesh::getVertexBuffer()
{
	return &mVertexBuffer;
}

ID3D11Buffer** StaticMesh::getWorldMatrixBuffer()
{
	return &mWorldMatrixBuffer;
}

void StaticMesh::setWorldMatrix(DirectX::XMMATRIX matrix)
{
	DirectX::XMStoreFloat4x4(&mWorldMatrix, matrix);
}

DirectX::XMFLOAT4X4* StaticMesh::getWorldMatrix()
{
	return &mWorldMatrix;
}

void StaticMesh::setMaterial(DataStructures::LambertData material, std::wstring texturePath)
{
	mMaterial.data = material;

	if (texturePath.length() > 0) {
		if (mMaterial.srv != nullptr)
			mMaterial.srv->Release();

		HRESULT hr = DirectX::CreateDDSTextureFromFile(
			GraphicsManager::getInstance().getDevice(),
			texturePath.c_str(),
			NULL,
			&mMaterial.srv
			);

		if (SUCCEEDED(hr))
			mMaterial.hasTexture = true;
	}
}

DataStructures::LambertMaterial* StaticMesh::getMaterial()
{
	return &mMaterial;
}