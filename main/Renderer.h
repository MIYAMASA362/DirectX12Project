#ifndef __RENDERER_H__
#define __RENDERER_H__

#include "Object.h"

using namespace DirectX;
using namespace Microsoft::WRL;

struct Vertex3D
{
	XMFLOAT3 position;
	XMFLOAT3 normal;
	XMFLOAT2 texCoord;
	XMFLOAT4 color;
};

// static_assert((sizeof(ConstantBuffer) % 256) == 0, "Constant Buffer size must be 256-byte aligned");
_declspec(align(256u)) struct ObjectConstantBuffer
{
	XMFLOAT4X4 world;
};

class Plane : public GameObject
{
public:
	void onSetup();
	void onUpdate();
	void onRender();
};

class DescriptorHeap
{
public:
	HRESULT Create(const D3D12_DESCRIPTOR_HEAP_DESC* pDesc, ID3D12Device** ppDevice);

	ID3D12DescriptorHeap* GetHeap() const { return mHeap.Get(); }
	UINT GetSize() const { return mSize; }

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(UINT offset);
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(UINT offset);
private:
	ComPtr<ID3D12DescriptorHeap> mHeap;
	UINT mSize;
};

class Renderer final
{
public:
	static Renderer* getInstance();

	Renderer();
	~Renderer();

	ComPtr<ID3D12Device>& getD3DDevice() { return mDevice; }

	void onInit();
	void onRender(class Camera* pCamera);
	void onDestroy();

	void onRegisterDataBuffer(int slot, void* pData, size_t size);

private:
	void createHardwareAdapter(IDXGIFactory4* pFactory, IDXGIAdapter** ppAdapter, bool useWarpDevice, D3D_FEATURE_LEVEL featureLevel, bool requestHighPerformanceAdapter);
	void createDevice(const D3D_FEATURE_LEVEL& featureLevel);

	void loadPipeline();
	void loadPipelineAssets();
	void loadRootSignature();
	void loadPipelineState();

	void setResourceDataPtr();
	void setDescriptorResource();

	void createCommandQueue();
	void createSwapChain(IDXGIFactory4* facotry);
	void createCommandAllocator();
	void createCommandList();
	void createSyncObject();
	void createPipelineAssets();
	void createDescriptorHeap();

	void createAssets();

	void begin();
	void record(class Camera* pCamera);
	void end();

	void resetCommandList(ID3D12CommandAllocator* const allocator);
	void populateCommandList();

	// Wait for pending GPU work to complete.
	void waitForGpu();
	// Prepare to render the next frame.
	void moveToNextFrame();

#if defined(_DEBUG)
	void enableDebugLayer(UINT& dxgiFactoryFlags);
#endif

private:
	static const UINT FrameCount = 2;
	static Renderer* gInstance;

	bool								mUseWarpDevice;

private:
	ComPtr<IDXGIFactory4>				mFactory;
	ComPtr<IDXGIAdapter>				mAdapter;
	ComPtr<ID3D12Device>				mDevice;
	ComPtr<IDXGISwapChain3>				mSwapChain;
	ComPtr<ID3D12CommandQueue>			mCommandQueue;
	ComPtr<ID3D12CommandAllocator>		mCommandAllocators[FrameCount];
	ComPtr<ID3D12RootSignature>			mRootSignature;

private:
	DescriptorHeap mRTVHeap;
	DescriptorHeap mDSVHeap;
	DescriptorHeap mCBVHeap;
	DescriptorHeap mSRVHeap;

	ComPtr<ID3D12Resource> mRenderTargets[FrameCount];
	ComPtr<ID3D12Resource> mDepthStencil;
	ComPtr<ID3D12Resource> mObjectConstantBuffer;
	ComPtr<ID3D12Resource> mSceneConstantBuffer;
	ComPtr<ID3D12Resource> mVertexBuffer;
	ComPtr<ID3D12Resource> mIndexBuffer;

	ComPtr<ID3D12CommandAllocator>		mBundleAllocator;

	UINT								mFrameIndex;

	// Asset objects
	ComPtr<ID3D12PipelineState>			mPSOGeometory;
	ComPtr<ID3D12GraphicsCommandList>	mCommandList;
	ComPtr<ID3D12GraphicsCommandList>	mBundle;

	UINT8* mDataPtr[2];

	D3D12_VERTEX_BUFFER_VIEW			mVertexBufferView;
	D3D12_INDEX_BUFFER_VIEW				mIndexBufferView;
	UINT								mIndexCount;

	// Synchronization objects
	HANDLE								mSwapChainEvent;
	ComPtr<ID3D12Fence>					mFences[FrameCount];
	UINT64								mFenceValues[FrameCount];
	HANDLE								mFenceEvent;
};


#endif 