#ifndef __OBJECT_CUBE_H__
#define __OBJECT_CUBE_H__

#include "Object.h"

class Cube : public Object
{
public:
	Cube();
	virtual ~Cube();

	virtual void onInit();
	virtual void onUpdate();
	virtual void onDraw(ID3D12GraphicsCommandList* commandList);
	virtual void onDestroy();

private:
	ComPtr<ID3D12Resource> mVertexBuffer;
	ComPtr<ID3D12Resource> mIndexBuffer;
	ComPtr<ID3D12Resource> mConstantBuffer;

	D3D12_VERTEX_BUFFER_VIEW mVertexBufferView;
	D3D12_INDEX_BUFFER_VIEW mIndexBufferView;

	XMFLOAT3 mPosition;
	XMFLOAT3 mRotation;
	XMFLOAT3 mScale;
};

#endif
