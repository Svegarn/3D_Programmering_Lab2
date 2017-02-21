#include "Camera.h"

Camera::~Camera()
{
}

void Camera::Initialize(UINT windowWidth, UINT windowHeight)
{
	mWindowWidth = windowWidth;
	mWindowHeight = windowHeight;

	mFovY = float(DirectX::XM_PI / 180.0f) * 45.0f;
	mAspect = (float)mWindowWidth / (float)mWindowHeight;
	mNearZ = 0.1f;
	mFarZ = 20000.0f;

	SetLens();
}

void Camera::Update(float deltaTime)
{
	if (GetAsyncKeyState(0x57)) // W | Forward
		Walk(moveSpeed * deltaTime);
	if (GetAsyncKeyState(0x53)) // S | Backwards
		Walk(-moveSpeed * deltaTime);
	if (GetAsyncKeyState(0x44)) // D | Strafe right
		Strafe(moveSpeed * deltaTime);
	if (GetAsyncKeyState(0x41)) // A | Strafe left
		Strafe(-moveSpeed * deltaTime);
	if (GetAsyncKeyState(VK_SPACE)) // Space | Up
		Levitate(moveSpeed * deltaTime);
	if (GetAsyncKeyState(0x43)) // C | Down
		Levitate(-moveSpeed * deltaTime);

	if (GetAsyncKeyState(VK_LSHIFT)) // Shift | Sprint
		moveSpeed = sprintSpeed;
	else
		moveSpeed = baseSpeed;

	mRotX = 0.0f;
	mRotY = 0.0f;

	UpdateViewMatrix();
}

void Camera::Walk(float distance)
{
	DirectX::XMVECTOR s = DirectX::XMVectorReplicate(distance);
	DirectX::XMVECTOR l = DirectX::XMLoadFloat3(&mLook);
	DirectX::XMVECTOR p = DirectX::XMLoadFloat3(&mPosition);
	DirectX::XMStoreFloat3(&mPosition, DirectX::XMVectorMultiplyAdd(s, l, p));
}

void Camera::Strafe(float distance)
{
	DirectX::XMVECTOR s = DirectX::XMVectorReplicate(distance);
	DirectX::XMVECTOR r = DirectX::XMLoadFloat3(&mRight);
	DirectX::XMVECTOR p = DirectX::XMLoadFloat3(&mPosition);
	DirectX::XMStoreFloat3(&mPosition, DirectX::XMVectorMultiplyAdd(s, r, p));
}

void Camera::Levitate(float distance)
{
	DirectX::XMVECTOR s = DirectX::XMVectorReplicate(distance);
	DirectX::XMVECTOR u = DirectX::XMLoadFloat3(&mUp);
	DirectX::XMVECTOR p = DirectX::XMLoadFloat3(&mPosition);
	DirectX::XMStoreFloat3(&mPosition, DirectX::XMVectorMultiplyAdd(s, u, p));
}

void Camera::Pitch(float angle)
{
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationAxis(DirectX::XMLoadFloat3(&mRight), angle);

	XMStoreFloat3(&mUp, DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&mUp), R));
	XMStoreFloat3(&mLook, DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&mLook), R));
}

void Camera::RotateY(float angle)
{
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationY(angle);

	DirectX::XMStoreFloat3(&mRight, DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&mRight), R));
	DirectX::XMStoreFloat3(&mUp, DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&mUp), R));
	DirectX::XMStoreFloat3(&mLook, DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&mLook), R));
}

void Camera::UpdateViewMatrix()
{
	DirectX::XMVECTOR R = DirectX::XMLoadFloat3(&mRight);
	DirectX::XMVECTOR U = DirectX::XMLoadFloat3(&mUp);
	DirectX::XMVECTOR L = DirectX::XMLoadFloat3(&mLook);
	DirectX::XMVECTOR P = DirectX::XMLoadFloat3(&mPosition);

	// Keep camera's axes orthogonal to each other and of unit length.
	L = DirectX::XMVector3Normalize(L);
	U = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(L, R));

	// U, L already ortho-normal, so no need to normalize cross product.
	R = DirectX::XMVector3Cross(U, L);

	// Fill in the view matrix entries.
	float x = -DirectX::XMVectorGetX(DirectX::XMVector3Dot(P, R));
	float y = -DirectX::XMVectorGetX(DirectX::XMVector3Dot(P, U));
	float z = -DirectX::XMVectorGetX(DirectX::XMVector3Dot(P, L));

	DirectX::XMStoreFloat3(&mRight, R);
	DirectX::XMStoreFloat3(&mUp, U);
	DirectX::XMStoreFloat3(&mLook, L);

	mView(0, 0) = mRight.x;
	mView(1, 0) = mRight.y;
	mView(2, 0) = mRight.z;
	mView(3, 0) = x;

	mView(0, 1) = mUp.x;
	mView(1, 1) = mUp.y;
	mView(2, 1) = mUp.z;
	mView(3, 1) = y;

	mView(0, 2) = mLook.x;
	mView(1, 2) = mLook.y;
	mView(2, 2) = mLook.z;
	mView(3, 2) = z;

	mView(0, 3) = 0.0f;
	mView(1, 3) = 0.0f;
	mView(2, 3) = 0.0f;
	mView(3, 3) = 1.0f;
}

void Camera::MouseMove(int x, int y, float deltaTime)
{
	if (x != mLastMousePos.x || y != mLastMousePos.y)
	{
		if (x != mLastMousePos.x)
		{
			mRotX += ((mouseSensitivity * deltaTime) * DirectX::XMConvertToRadians(x - mLastMousePos.x));
			RotateY(mRotX);
		}
	
		if (y != mLastMousePos.y)
		{
			mRotY += ((mouseSensitivity * deltaTime) * DirectX::XMConvertToRadians(y - mLastMousePos.y));
			Pitch(mRotY);
		}
	
		SetCursorPos(mWindowWidth / 2, mWindowHeight / 2);
	
		mLastMousePos.x = x;
		mLastMousePos.y = y;
	}
}

void Camera::SetLens()
{
	DirectX::XMMATRIX P = DirectX::XMMatrixPerspectiveFovLH(mFovY, mAspect, mNearZ, mFarZ);
	DirectX::XMStoreFloat4x4(&mProjection, P);
}

DirectX::XMFLOAT4X4* Camera::getView()
{
	return &mView;
}

DirectX::XMFLOAT4X4* Camera::getProjection()
{
	return &mProjection;
}

DirectX::XMFLOAT3* Camera::getPosition()
{
	return &mPosition;
}