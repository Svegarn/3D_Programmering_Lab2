#include "StaticMesh.h"

StaticMesh::StaticMesh(std::string filePath, DirectX::XMMATRIX worldMatrix)
{
	FbxManager* manager = FbxManager::Create();
	FbxIOSettings *ios = FbxIOSettings::Create(manager, IOSROOT);
	manager->SetIOSettings(ios);
	
	// Create an importer using the SDK manager.
	FbxImporter* importer = FbxImporter::Create(manager, "");
	
	// Use the first argument as the filename for the importer.
	if (!importer->Initialize(filePath.c_str(), -1, manager->GetIOSettings()))
		exit(-1);
	
	// Create a new scene so that it can be populated by the imported file.
	FbxScene* scene = FbxScene::Create(manager, "newScene");
	
	// Import the contents of the file into the scene.
	importer->Import(scene);
	
	// The file is imported, so get rid of the importer.
	importer->Destroy();

	// Get mesh node
	FbxMesh* mesh = nullptr;

	std::vector<FbxNode*> dagNodes;
	FbxNode* root = scene->GetRootNode();
	
	if (strcmp(root->GetChild(0)->GetTypeName(), "Mesh") == 0)
	{
		mesh = root->GetChild(0)->GetMesh();
	}
	else
	{
		OutputDebugString(L"Non-mesh object found");
		exit(-1);
	}

	// Get transformation
	FbxDouble3 rotation = root->GetChild(0)->LclRotation.Get();
	FbxDouble3 scaling = root->GetChild(0)->LclScaling.Get();
	FbxDouble3 translation = root->GetChild(0)->LclTranslation.Get();

	DirectX::XMVECTOR rotationQuat = DirectX::XMQuaternionRotationRollPitchYaw(rotation[0], rotation[1], rotation[2]);

	DirectX::XMMATRIX transformation = DirectX::XMMatrixTransformation(
		DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f),
		DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
		DirectX::XMVectorSet(scaling[0], scaling[1], scaling[2], 1.0f),
		DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
		rotationQuat,
		DirectX::XMVectorSet(translation[0], translation[1], translation[2], 1.0f)
		);

	DirectX::XMStoreFloat4x4(&mWorldMatrix, transformation);
	
	// Get mesh data
	FbxVector4* controlPoints = mesh->GetControlPoints();
	FbxVector4 normal;
	FbxVector2 uv;
	bool unmapped = false;
	
	int numVertices = mesh->GetControlPointsCount();
	int numPolygons = mesh->GetPolygonCount();
	int polySize = 0;
	int vertexIndex = 0;

	FbxLayerElementArrayTemplate<FbxVector2>* uvArray;
	mesh->GetTextureUV(&uvArray);

	vertexCount = mesh->GetPolygonVertexCount();
	DataStructures::Vertex* vertexList = new DataStructures::Vertex[vertexCount];
	int counter = 0;
	for (UINT i = 0; i < numPolygons; i++) {
		polySize = mesh->GetPolygonSize(i);

		for (UINT j = 0; j < polySize; j++) {
			vertexIndex = mesh->GetPolygonVertex(i, j);
			
			vertexList[counter].position = {
				(float)controlPoints[vertexIndex][0],
				(float)controlPoints[vertexIndex][1],
				(float)controlPoints[vertexIndex][2]
			};

			mesh->GetPolygonVertexNormal(i, j, normal);
			vertexList[counter].normal = {
				(float)normal[0],
				(float)normal[1],
				(float)normal[2]
			};

			vertexList[counter].uv = {
				(float)uvArray->GetAt(mesh->GetTextureUVIndex(i, j))[0],
				(float)uvArray->GetAt(mesh->GetTextureUVIndex(i, j))[1]
			};
			counter++;
		}
	}

	// Destroy the SDK manager and all the other objects it was handling.
	manager->Destroy();

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

	delete[] vertexList;
}

StaticMesh::StaticMesh(DataStructures::Vertex* vertexList, UINT vertexCount, DirectX::XMMATRIX worldMatrix)
{
	this->vertexCount = vertexCount;
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
	vertexDesc.ByteWidth = this->vertexCount * sizeof(DataStructures::Vertex);

	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = vertexList;
	GraphicsManager::getInstance().getDevice()->CreateBuffer(&vertexDesc, &vertexData, &mVertexBuffer);
}

StaticMesh::StaticMesh(std::vector<DataStructures::Vertex> &vertexList, DirectX::XMMATRIX worldMatrix)
{
	this->vertexCount = vertexList.size();
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
	vertexDesc.ByteWidth = this->vertexCount * sizeof(DataStructures::Vertex);

	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = vertexList.data();
	GraphicsManager::getInstance().getDevice()->CreateBuffer(&vertexDesc, &vertexData, &mVertexBuffer);
}

StaticMesh::~StaticMesh()
{
	mVertexBuffer->Release();
	mWorldMatrixBuffer->Release();

	if(mMaterial.srv != nullptr)
		mMaterial.srv->Release();
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