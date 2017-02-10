#ifndef DATASTRUCTURES_H
#define DATASTRUCTURES_H

#include <string>
#include <DirectXMath.h>
#include <vector>

namespace DataStructures
{
	enum MaterialType {
		MATERIAL_LAMBERT,
		MATERIAL_PHONG
	};

	enum MeshType {
		MESH_STATIC,
		MESH_DYNAMIC
	};

	struct LambertData {
		float color[3] = { 0.0f, 0.0f, 0.0f };
		float padding = 0;
	};

	struct LambertMaterial {
		LambertData data;
		ID3D11ShaderResourceView* srv = nullptr;
		bool hasTexture = false;
	};

	struct PhongMaterial {
		float color[3] = { 0.0f, 0.0f, 0.0f };
		float specular = 1;
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
		DirectX::XMFLOAT4 DT;
		DirectX::XMFLOAT4 eyePos;
	};
}

#endif