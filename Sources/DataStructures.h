#ifndef DATASTRUCTURES_H
#define DATASTRUCTURES_H

#include <string>
#include <DirectXMath.h>
#include <vector>

namespace DataStructures
{
	enum Materials {
		MATERIAL_LAMBERT1,
		MATERIAL_PHONG1
	};

	enum MeshType {
		MESH_STATIC,
		MESH_DYNAMIC
	};

	struct Vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT2 uv;
	};

	struct CameraCbuffer {
		DirectX::XMFLOAT4X4 view;
		DirectX::XMFLOAT4X4 projection;
		DirectX::XMFLOAT4X4 viewInverse;
		DirectX::XMFLOAT4X4 projectionInverse;
		DirectX::XMFLOAT4 DT; // DeltaTime, ElapsedTime, Padding, Padding
		DirectX::XMFLOAT4 eyePos;
	};

	struct MaterialData {
		DirectX::XMFLOAT4 color = { 0.0f, 0.0f, 0.0f, 1.0f }; // R = 0.0f, G = 0.0f, B = 0.0f, A = 1.0f
		DirectX::XMFLOAT4 specular = { 0.0f, 0.0f, 0.0f, 1.0f }; // R = 0.0f, G = 0.0f, B = 0.0f, Shinyness = 1.0f
		DirectX::XMFLOAT4 misc = { 0.0f, 0.0f, 0.0f, 0.0f }; // uvAnimationSpeed = 0.0f, Padding = 0.0f, Padding = 0.0f, Padding = 0.0f
		DirectX::XMINT4 hasTexture = { 0, 0, 0, 0 }; // hasTexture = 0, Padding = 0, Padding = 0, Padding = 0
	};

	struct Material {
		MaterialData data;
		ID3D11ShaderResourceView* srv = nullptr;
		ID3D11Buffer* cBuffer = nullptr;
	};

}

#endif