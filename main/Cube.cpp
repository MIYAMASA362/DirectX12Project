#include "stdafx.h"
#include "Renderer.h"
#include "Cube.h"

Cube::Cube()
	: mVertexBuffer()
	, mIndexBuffer()
	, mConstantBuffer()
	, mPosition()
	, mRotation()
	, mScale()
{

}

Cube::~Cube()
{

}

void Cube::onInit()
{
	ComPtr<ID3D12Device>& device = CRenderer::getInstance()->getD3DDevice();

	//
	D3D12_HEAP_PROPERTIES heapProp{};
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapProp.CreationNodeMask = 0;
	heapProp.VisibleNodeMask = 0;

	//
	D3D12_RESOURCE_DESC resDesc{};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.Format = DXGI_FORMAT_UNKNOWN;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	resDesc.SampleDesc.Count = 1;
	resDesc.SampleDesc.Quality = 0;



	// 
	resDesc.Width = sizeof(Vertex3D) * 24;
	ThrowIfFailed(device->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &resDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&mVertexBuffer)));

	Vertex3D* bufferBegin;
	ThrowIfFailed(mVertexBuffer->Map(0, nullptr, (void**)&bufferBegin));

	//up
	bufferBegin[0] = { {-1.0f,  1.0f, 1.0f},	{ 0.0f, 1.0f,0.0f},	 {0.0f,0.0f}, {1.0f, 1.0f, 1.0f, 1.0f} };
	bufferBegin[1] = { { 1.0f,  1.0f, 1.0f},	{ 0.0f, 1.0f,0.0f},	 {1.0f,0.0f}, {1.0f, 1.0f, 1.0f, 1.0f} };
	bufferBegin[2] = { {-1.0f,  1.0f,-1.0f},	{ 0.0f, 1.0f,0.0f},	 {0.0f,1.0f}, {1.0f, 1.0f, 1.0f, 1.0f} };
	bufferBegin[3] = { { 1.0f,  1.0f,-1.0f},	{ 0.0f, 1.0f,0.0f},	 {1.0f,1.0f}, {1.0f, 1.0f, 1.0f, 1.0f} };

	//down															 
	bufferBegin[4] = { { 1.0f, -1.0f, 1.0f},	{ 0.0f,-1.0f,0.0f},	 {0.0f,0.0f}, {1.0f, 1.0f, 1.0f, 1.0f} };
	bufferBegin[5] = { {-1.0f, -1.0f, 1.0f},	{ 0.0f,-1.0f,0.0f},	 {1.0f,0.0f}, {1.0f, 1.0f, 1.0f, 1.0f} };
	bufferBegin[6] = { { 1.0f, -1.0f,-1.0f},	{ 0.0f,-1.0f,0.0f},	 {0.0f,1.0f}, {1.0f, 1.0f, 1.0f, 1.0f} };
	bufferBegin[7] = { {-1.0f, -1.0f,-1.0f},	{ 0.0f,-1.0f,0.0f},	 {1.0f,1.0f}, {1.0f, 1.0f, 1.0f, 1.0f} };

	//foward
	bufferBegin[8] = { { 1.0f,  1.0f, 1.0f},	{ 0.0f, 0.0f, 1.0f}, {0.0f,0.0f}, {1.0f, 1.0f, 1.0f, 1.0f} };
	bufferBegin[9] = { {-1.0f,  1.0f, 1.0f},	{ 0.0f, 0.0f, 1.0f}, {1.0f,0.0f}, {1.0f, 1.0f, 1.0f, 1.0f} };
	bufferBegin[10] = { { 1.0f, -1.0f, 1.0f},	{ 0.0f, 0.0f, 1.0f}, {0.0f,1.0f}, {1.0f, 1.0f, 1.0f, 1.0f} };
	bufferBegin[11] = { {-1.0f, -1.0f, 1.0f},	{ 0.0f, 0.0f, 1.0f}, {1.0f,1.0f}, {1.0f, 1.0f, 1.0f, 1.0f} };

	//back															 
	bufferBegin[12] = { {-1.0f,  1.0f,-1.0f},	{ 0.0f, 0.0f,-1.0f}, {0.0f,0.0f}, {1.0f, 1.0f, 1.0f, 1.0f} };
	bufferBegin[13] = { { 1.0f,  1.0f,-1.0f},	{ 0.0f, 0.0f,-1.0f}, {1.0f,0.0f}, {1.0f, 1.0f, 1.0f, 1.0f} };
	bufferBegin[14] = { {-1.0f, -1.0f,-1.0f},	{ 0.0f, 0.0f,-1.0f}, {0.0f,1.0f}, {1.0f, 1.0f, 1.0f, 1.0f} };
	bufferBegin[15] = { { 1.0f, -1.0f,-1.0f},	{ 0.0f, 0.0f,-1.0f}, {1.0f,1.0f}, {1.0f, 1.0f, 1.0f, 1.0f} };

	//left															 
	bufferBegin[16] = { {-1.0f,  1.0f, 1.0f},	{-1.0f, 0.0f, 0.0f}, {0.0f,0.0f}, {1.0f, 1.0f, 1.0f, 1.0f} };
	bufferBegin[17] = { {-1.0f,  1.0f,-1.0f},	{-1.0f, 0.0f, 0.0f}, {1.0f,0.0f}, {1.0f, 1.0f, 1.0f, 1.0f} };
	bufferBegin[18] = { {-1.0f, -1.0f, 1.0f},	{-1.0f, 0.0f, 0.0f}, {0.0f,1.0f}, {1.0f, 1.0f, 1.0f, 1.0f} };
	bufferBegin[19] = { {-1.0f, -1.0f,-1.0f},	{-1.0f, 0.0f, 0.0f}, {1.0f,1.0f}, {1.0f, 1.0f, 1.0f, 1.0f} };

	//right															 
	bufferBegin[20] = { { 1.0f,  1.0f,-1.0f},	{ 1.0f, 0.0f, 0.0f}, {0.0f,0.0f}, {1.0f, 1.0f, 1.0f, 1.0f} };
	bufferBegin[21] = { { 1.0f,  1.0f, 1.0f},	{ 1.0f, 0.0f, 0.0f}, {1.0f,0.0f}, {1.0f, 1.0f, 1.0f, 1.0f} };
	bufferBegin[22] = { { 1.0f, -1.0f,-1.0f},	{ 1.0f, 0.0f, 0.0f}, {0.0f,1.0f}, {1.0f, 1.0f, 1.0f, 1.0f} };
	bufferBegin[23] = { { 1.0f, -1.0f, 1.0f},	{ 1.0f, 0.0f, 0.0f}, {1.0f,1.0f}, {1.0f, 1.0f, 1.0f, 1.0f} };

	mVertexBuffer->Unmap(0, nullptr);

	mVertexBufferView.BufferLocation = mVertexBuffer->GetGPUVirtualAddress();
	mVertexBufferView.StrideInBytes = sizeof(Vertex3D);
	mVertexBufferView.SizeInBytes = resDesc.Width;


	// 
	resDesc.Width = sizeof(UINT) * 36;
	ThrowIfFailed(device->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &resDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&mIndexBuffer)));

	UINT* indexBegin;
	ThrowIfFailed(mIndexBuffer->Map(0, nullptr, (void**)&indexBegin));

	for (UINT i = 0; i < 6; ++i)
	{
		indexBegin[i * 6 + 0] = i * 4 + 0;
		indexBegin[i * 6 + 1] = i * 4 + 1;
		indexBegin[i * 6 + 2] = i * 4 + 2;

		indexBegin[i * 6 + 3] = i * 4 + 1;
		indexBegin[i * 6 + 4] = i * 4 + 3;
		indexBegin[i * 6 + 5] = i * 4 + 2;
	}

	mIndexBuffer->Unmap(0, nullptr);

	mIndexBufferView.BufferLocation = mIndexBuffer->GetGPUVirtualAddress();
	mIndexBufferView.SizeInBytes = resDesc.Width;
	mIndexBufferView.Format = DXGI_FORMAT_R32_UINT;


	resDesc.Width = sizeof(ConstantBuffer);
	ThrowIfFailed(device->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &resDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&mConstantBuffer)));

}

void Cube::onUpdate()
{
	mRotation.x += 0.01f;
	mRotation.y += 0.005f;
}

void Cube::onDraw(ID3D12GraphicsCommandList* commandList)
{
	D3D12_VIEWPORT viewport = CRenderer::getInstance()->getViewport();

	XMMATRIX view = XMMatrixLookAtLH({ 0.0f,4.0f,-10.0f }, { 0.0f,0.0f,0.0f }, { 0.0f,1.0f,0.0f });
	XMMATRIX projection = XMMatrixPerspectiveFovLH(1.0f, viewport.Width / viewport.Height, 1.0f, 100.0f);
	XMMATRIX world = XMMatrixTranslation(0.0f, 0.0f, 0.0f);

	world *= XMMatrixRotationRollPitchYaw(mRotation.x,mRotation.y,mRotation.z);

	ConstantBuffer* constant;
	ThrowIfFailed(mConstantBuffer->Map(0, nullptr, (void**)&constant));

	XMFLOAT4X4 matrix;
	XMStoreFloat4x4(&matrix, XMMatrixTranspose(world * view * projection));
	constant->wvp = matrix;
	XMStoreFloat4x4(&matrix, XMMatrixTranspose(world));
	constant->world = matrix;

	mConstantBuffer->Unmap(0,nullptr);

	PIXBeginEvent(commandList, 0, L"Draw Cube");
	{
		//commandList->SetGraphicsRootConstantBufferView(0,mConstantBuffer->GetGPUVirtualAddress());
		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		commandList->IASetVertexBuffers(0,1,&mVertexBufferView);
		commandList->IASetIndexBuffer(&mIndexBufferView);

		commandList->DrawIndexedInstanced(36,1,0,0,0);
	}
	PIXEndEvent(commandList);
}

void Cube::onDestroy()
{

}
