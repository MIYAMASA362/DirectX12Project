#include "stdafx.h"
#include "Renderer.h"
#include "Camera.h"
#include "Application.h"
#include "Input.h"

#include "Math.h"

#define CAMERA_SPEED 0.01f;

float gSpeedScale = 2.0f;

Camera::Camera()
	: mNumViewport(1)
	, mViewport()
	, mScissorRect()
	, mView()
	, mProjection()
	, mClearColor()
{

}

Camera::~Camera()
{

}

void Camera::setup()
{
	LONG width, height;
	RECT rect;
	GetWindowRect(Application::getHwnd(), &rect);

	width = rect.right - rect.left;
	height = rect.bottom - rect.top;

	// ビューポート設定
	{
		// Viewport
		mViewport.TopLeftX = 0;
		mViewport.TopLeftY = 0;
		mViewport.Width = (FLOAT)width;
		mViewport.Height = (FLOAT)height;
		mViewport.MinDepth = 0.0f;
		mViewport.MaxDepth = 1.0f;

		// ScissorRect
		mScissorRect.left = 0;
		mScissorRect.top = 0;
		mScissorRect.right = width;
		mScissorRect.bottom = height;
	}

	setClearColor(0.0f, 0.2f, 0.4f, 1.0f);
}

void Camera::update()
{
	Input* input = Input::getInstance();

	float speed = CAMERA_SPEED;
	if (input->getKey(VK_LSHIFT)) {
		speed *= gSpeedScale;
		gSpeedScale += input->getAcceleration().z * 0.001f;
		gSpeedScale = gSpeedScale < 0 ? 0 : gSpeedScale;
	}

	Vector3 position = getTransform()->getLocalPosition();
	Quaternion rotation = getTransform()->getLocalRotation();

	// 方向取得計算
	XMMATRIX world = XMMatrixIdentity();
	world *= XMMatrixRotationQuaternion(rotation);
	world *= XMMatrixTranslationFromVector(position);

	XMFLOAT3 right(XMVector3Normalize(world.r[0]).m128_f32);
	XMFLOAT3 up(XMVector3Normalize(world.r[1]).m128_f32);
	XMFLOAT3 forward(XMVector3Normalize(world.r[2]).m128_f32);

	if (input->getMouseButton(1)) {
		rotation *= quaternion::AxisToEuler(input->getAcceleration().x * 0.025f, vector3::YAxis);
		rotation *= quaternion::AxisToEuler(input->getAcceleration().y * 0.025f, vector3::XAxis);
		input->setCursorLoop(true);
	}
	else {
		input->setCursorLoop(false);
	}

	if (input->getKey('W')) {
		position.x += forward.x * speed;
		position.y += forward.y * speed;
		position.z += forward.z * speed;
	}
	if (input->getKey('S')) {
		position.x -= forward.x * speed;
		position.y -= forward.y * speed;
		position.z -= forward.z * speed;
	}

	if (input->getKey('A')) {
		position.x -= right.x * speed;
		position.y -= right.y * speed;
		position.z -= right.z * speed;
	}
	if (input->getKey('D')) {
		position.x += right.x * speed;
		position.y += right.y * speed;
		position.z += right.z * speed;
	}

	if (input->getKey('E')) {
		position.x += up.x * speed;
		position.y += up.y * speed;
		position.z += up.z * speed;
	}
	if (input->getKey('Q')) {
		position.x -= up.x * speed;
		position.y -= up.y * speed;
		position.z -= up.z * speed;
	}

	getTransform()->setLocalPosition(position);
	getTransform()->setLocalRotation(rotation);
}

void Camera::onRender()
{
	XMVECTOR det;
	mView = XMMatrixInverse(&det, getTransform()->getWorldMatrix());
	mProjection = XMMatrixPerspectiveFovLH(1.0f, mViewport.Width / mViewport.Height, 1.0f, 100.0f);

	CameraConstantBuffer buffer;

	XMFLOAT4X4 matrix;
	XMStoreFloat4x4(&matrix, XMMatrixTranspose(mView));
	buffer.view = matrix;
	XMStoreFloat4x4(&matrix, XMMatrixTranspose(mProjection));
	buffer.projection = matrix;

	Renderer::getInstance()->onRegisterDataBuffer(1, &buffer, sizeof(CameraConstantBuffer));
}
