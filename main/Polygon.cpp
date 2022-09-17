#include "stdafx.h"
#include "Polygon.h"
#include "Renderer.h"
#include "Application.h"

Polygon::Polygon()
{
}

Polygon::~Polygon()
{
}

void Polygon::onInit()
{
	ComPtr<ID3D12Device>& device = CRenderer::getInstance()->getD3DDevice();

	D3D12_HEAP_PROPERTIES heapProp{};
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapProp.CreationNodeMask = 0;
	heapProp.VisibleNodeMask = 0;

	D3D12_RESOURCE_DESC resDesc{};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Alignment = 0;
	resDesc.Width = sizeof(Constant);
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.Format = DXGI_FORMAT_UNKNOWN;
	resDesc.SampleDesc.Count = 1;
	resDesc.SampleDesc.Quality = 0;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	ThrowIfFailed(device->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&mConstantBuffer))
	);

	
}

void Polygon::onUpdate()
{

}

void Polygon::onDraw(ID3D12GraphicsCommandList* commandList)
{
	D3D12_VIEWPORT viewPort = CRenderer::getInstance()->getViewport();

	XMMATRIX view = XMMatrixIdentity();
	XMMATRIX projection = XMMatrixOrthographicOffCenterLH(0.0f,viewPort.Width,viewPort.Height,0,0,1.0f);
	XMMATRIX world = XMMatrixTranslation(0.0f,0.0f,0.0f);

	XMVECTOR rot = { mRotation.x,mRotation.y,mRotation.z };
	world *= XMMatrixRotationRollPitchYawFromVector(rot);

	Constant* constant;
	ThrowIfFailed(mConstantBuffer->Map(0,nullptr,(void**)&constant));

	XMFLOAT4X4 matrix;
	XMStoreFloat4x4(&matrix, XMMatrixTranspose(world * view * projection));
	constant->wvp = matrix;
	XMStoreFloat4x4(&matrix, XMMatrixTranspose(world));
	constant->world = matrix;

	mConstantBuffer->Unmap(0,nullptr);

	commandList->SetGraphicsRootConstantBufferView(0, mConstantBuffer->GetGPUVirtualAddress());
}

void Polygon::onDestroy()
{
	mRotation.x += 0.01f;
	mRotation.y += 0.005f;
}
