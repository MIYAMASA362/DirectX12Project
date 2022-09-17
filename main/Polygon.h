#ifndef __OBJECT_POLYGON_H__
#define __OBJECT_POLYGON_H__

#include "IObject.h"

class Polygon : public IObject
{
public:
	struct Constant
	{
		XMFLOAT4X4 wvp;
		XMFLOAT4X4 world;
		float padding[32];
	};

public:
	Polygon();
	virtual ~Polygon();

	virtual void onInit() override;
	virtual void onUpdate() override;
	virtual void onDraw(ID3D12GraphicsCommandList* commandList) override;
	virtual void onDestroy() override;

private:
	ComPtr<ID3D12Resource> mVertexBuffer;
	ComPtr<ID3D12Resource> mIndexBuffer;

	ComPtr<ID3D12Resource> mConstantBuffer;

	XMFLOAT3 mRotation;
};

#endif
