#ifndef __OBJECT_CAMERA_H__
#define __OBJECT_CAMERA_H__

#include "Object.h"

using namespace DirectX;

_declspec(align(256u)) struct CameraConstantBuffer
{
	XMFLOAT4X4 view;
	XMFLOAT4X4 projection;
};

class Camera : public GameObject
{
public:
	Camera();
	virtual ~Camera();

	void setup();
	void update();
	void onRender();

	UINT getNumViewport() { return mNumViewport; }
	D3D12_VIEWPORT& getViewport() { return mViewport; }
	D3D12_RECT& getScissorRect() { return mScissorRect; }

	XMMATRIX getViewMatrix() const { return mView; }
	XMMATRIX getProjectionMatrix() const { return mProjection; }
	XMMATRIX getViewProjectionMatrix() const { return mView * mProjection; }

	void setClearColor(float r, float g, float b, float a) { mClearColor[0] = r; mClearColor[1] = g; mClearColor[2] = b; mClearColor[3] = a; }
	const float* getClearColor() { return mClearColor; }

private:
	UINT mNumViewport;
	D3D12_VIEWPORT mViewport;
	D3D12_RECT mScissorRect;

	XMMATRIX mView;
	XMMATRIX mProjection;

	float mClearColor[4];
};

#endif
