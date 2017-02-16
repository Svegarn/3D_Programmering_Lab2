#ifndef CAMERA_H
#define CAMERA_H

#include <DirectXMath.h>
#include <windows.h>

class Camera
{
private:
	Camera() {}
	~Camera();

	UINT mWindowWidth = 0;
	UINT mWindowHeight = 0;

	float mFovY = 0.0f;
	float mAspect = 0.0f;
	float mNearZ = 0.0f;
	float mFarZ = 0.0f;

	DirectX::XMFLOAT3 mRight = { 1.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT3 mUp = { 0.0f, 1.0f, 0.0f };
	DirectX::XMFLOAT3 mLook = { 0.0f, 0.0f, 1.0f };
	DirectX::XMFLOAT3 mPosition = { 0.0f, 0.0f, -2.0f };

	float moveSpeed = 0.0001f;

	POINT mLastMousePos;
	float mRotX = 0.0f;
	float mRotY = 0.0f;
	float mouseSensitivity = 0.05f;

	DirectX::XMFLOAT4X4 mView;
	DirectX::XMFLOAT4X4 mProjection;

	void Walk(float distance);
	void Strafe(float distance);
	void Levitate(float distance);

	void RotateY(float angle);
	void Pitch(float angle);

	void UpdateViewMatrix();
	void SetLens();

public:
	static Camera& getInstance()
	{
		static Camera instance;
		return instance;
	}

	Camera(Camera const&) = delete;
	void operator=(Camera const&) = delete;

	void Initialize(UINT windowWidth, UINT windowHeight);
	void MouseMove(int x, int y, float deltaTime);
	void Update(float deltaTime);
	
	DirectX::XMFLOAT4X4* getView();
	DirectX::XMFLOAT4X4* getProjection();
	DirectX::XMFLOAT3* getPosition();
};

#endif