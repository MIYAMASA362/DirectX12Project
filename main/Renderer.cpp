#include "stdafx.h"
#include <io.h>
#include <vector>
#include <fstream>

#include "Renderer.h"
#include "Application.h"

#include "Cube.h"

#include "Camera.h"

Renderer* Renderer::gInstance = nullptr;

Renderer* Renderer::getInstance()
{
	return gInstance;
}

Renderer::Renderer()
	: mUseWarpDevice(false)
	// Pipeline objects
	, mFactory(nullptr)
	, mDevice(nullptr)
	, mSwapChain(nullptr)
	, mCommandQueue(nullptr)
	, mCommandAllocators()
	, mBundleAllocator(nullptr)
	, mRootSignature(nullptr)

	// DescriptorHeap
	, mRTVHeap()
	, mCBVHeap()
	, mSRVHeap()
	, mDSVHeap()

	, mRenderTargets()
	, mDepthStencil(nullptr)
	, mFrameIndex(0)

	// Asset objects
	, mPSOGeometory(nullptr)
	, mCommandList(nullptr)
	, mBundle(nullptr)
	, mObjectConstantBuffer()
	, mSceneConstantBuffer()
	, mVertexBuffer(nullptr)
	, mVertexBufferView()
	, mIndexBuffer(nullptr)
	, mIndexBufferView()
	, mIndexCount(0)

	// Synchronization objects
	, mSwapChainEvent(NULL)
	, mFences()
	, mFenceValues()
	, mFenceEvent(NULL)
{
	if (gInstance == nullptr)
	{
		gInstance = this;
	}
}

Renderer::~Renderer()
{

}

void Renderer::onInit()
{
	loadPipeline();
	loadPipelineAssets();

	createAssets();
}

void Renderer::onRegisterDataBuffer(int slot, void* pData, size_t size)
{
	memcpy(mDataPtr[slot], pData, size);
}

void Renderer::onRender(Camera* pCamera)
{
	try 
	{
		PIXBeginEvent(mCommandQueue.Get(), 0, L"Render");
		{
			begin();
			
			record(pCamera);

			end();

			// Execute the command list.
			ID3D12CommandList* const ppCommandLists[] = { mCommandList.Get() };
			mCommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
		}
		PIXEndEvent(mCommandQueue.Get());

		// Present the frame.
		// SyncInterval : 垂直同期待ちフレーム
		ThrowIfFailed(mSwapChain->Present(1, 0));

		moveToNextFrame();
	}
	catch (HrException& e) 
	{
		if (e.Error() == DXGI_ERROR_DEVICE_REMOVED || e.Error() == DXGI_ERROR_DEVICE_RESET)
		{
			moveToNextFrame();
		}
		else throw;
	}
}

void Renderer::onDestroy()
{
	// Ensure that the GPU is no longer referencing resources that are about to be
	// cleaned up by the destructor.
	waitForGpu();

	CloseHandle(mFenceEvent);
}



void Renderer::createHardwareAdapter(IDXGIFactory4* pFactory, IDXGIAdapter** ppAdapter, bool useWarpDevice, D3D_FEATURE_LEVEL featureLevel, bool requestHighPerformanceAdapter)
{
	ComPtr<IDXGIAdapter> adapter;

	if (useWarpDevice)
	{
		ThrowIfFailed(pFactory->EnumWarpAdapter(IID_PPV_ARGS(&adapter)));
	}
	else
	{
		ComPtr<IDXGIAdapter1> adapter1;

		ComPtr<IDXGIFactory6> factory6;
		if (SUCCEEDED(pFactory->QueryInterface(IID_PPV_ARGS(&factory6))))
		{
			//	EnumApaterByGpuPreference
			//		EnumAdapters1と違い、第二引数の REFIID riid の値によって、列挙の順序が変わってくる
			for (
				UINT adapterIndex = 0;
				DXGI_ERROR_NOT_FOUND != factory6->EnumAdapterByGpuPreference(
					adapterIndex,
					requestHighPerformanceAdapter == true ? DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE : DXGI_GPU_PREFERENCE_UNSPECIFIED,
					IID_PPV_ARGS(&adapter1));
				++adapterIndex)
			{
				//	Adapterの詳細
				DXGI_ADAPTER_DESC1 desc;
				adapter1->GetDesc1(&desc);

				//	ソフトウェアアダプターであった
				if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
				{
					continue;
				}

				// Check to see whether the adapter supports Direct3D 12, but don't create the
				// actual device yet.
				if (SUCCEEDED(D3D12CreateDevice(adapter1.Get(), featureLevel, _uuidof(ID3D12Device), nullptr)))
				{
					break;
				}
			}
		}
		else
		{
			for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != pFactory->EnumAdapters1(adapterIndex, &adapter1); ++adapterIndex)
			{
				DXGI_ADAPTER_DESC1 desc;
				adapter1->GetDesc1(&desc);

				if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
				{
					// Don't select the Basic Render Driver adapter.
					// If you want a software adapter, pass in "/warp" on the command line.
					continue;
				}

				// Check to see whether the adapter supports Direct3D 12, but don't create the
				// actual device yet.
				if (SUCCEEDED(D3D12CreateDevice(adapter1.Get(), featureLevel, _uuidof(ID3D12Device), nullptr)))
				{
					break;
				}
			}
		}

		ThrowIfFailed(adapter1.As(&adapter));
	}

	*ppAdapter = adapter.Detach();
}

void Renderer::createDevice(const D3D_FEATURE_LEVEL& featureLevel)
{
	ThrowIfFailed(D3D12CreateDevice(mAdapter.Get(), featureLevel, IID_PPV_ARGS(&mDevice)));

	// Check Shader Support
	D3D12_FEATURE_DATA_SHADER_MODEL shaderModel = { D3D_SHADER_MODEL_6_5 };
	if (FAILED(mDevice->CheckFeatureSupport(D3D12_FEATURE_SHADER_MODEL, &shaderModel, sizeof(shaderModel)))
		|| (shaderModel.HighestShaderModel < D3D_SHADER_MODEL_6_5))
	{
		OutputDebugStringA("ERROR: Shader Model 6.5 is not supported\n");
		throw std::exception("Shader Model 6.5 is not supported");
	}

	D3D12_FEATURE_DATA_D3D12_OPTIONS7 features = {};
	if (FAILED(mDevice->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS7, &features, sizeof(features)))
		|| (features.MeshShaderTier == D3D12_MESH_SHADER_TIER_NOT_SUPPORTED))
	{
		OutputDebugStringA("ERROR: Mesh Shaders aren't supported\n");
		throw std::exception("Mesh Shaders arent't supported");
	}
}

void Renderer::loadPipeline()
{
	// 機能レベル
	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_12_0;
	UINT dxgiFactoryFlags = 0;

#if defined(_DEBUG)
	enableDebugLayer(dxgiFactoryFlags);
#endif

	//	Enables creating DXGI objects.
	//	DXGIオブジェクトの生成を有効にする
	ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&mFactory)));

	// アダプターの生成
	createHardwareAdapter(mFactory.Get(), &mAdapter, mUseWarpDevice, featureLevel, false);

	// デバイスの生成
	createDevice(featureLevel);

	// コマンドキューの作成
	createCommandQueue();

	// スワップチェインの作成
	createSwapChain(mFactory.Get());

	// コマンドアロケータの作成
	createCommandAllocator();

	// コマンドリストの生成
	createCommandList();

	// 同期オブジェクトの作成
	createSyncObject();

	mCommandList->Close();
}

/// <summary>
/// Rendererに必要なアセット読み込み
/// </summary>
void Renderer::loadPipelineAssets()
{
	createDescriptorHeap();

	loadRootSignature();
	loadPipelineState();

	createPipelineAssets();
	setDescriptorResource();
	setResourceDataPtr();
}

/// <summary>
/// ディスクリプタヒープを設定
/// </summary>
void Renderer::createDescriptorHeap()
{
	// RenderTargetView
	{
		D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
		heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		heapDesc.NumDescriptors = FrameCount;
		heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		heapDesc.NodeMask = 0;
		ThrowIfFailed(mRTVHeap.Create(&heapDesc, mDevice.GetAddressOf()));
	}

	// ConstantBufferView
	{
		D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
		heapDesc.NumDescriptors = 2;
		heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		heapDesc.NodeMask = 0;
		ThrowIfFailed(mCBVHeap.Create(&heapDesc, mDevice.GetAddressOf()));
	}

	// ShaderResourceView
	{
		D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
		heapDesc.NumDescriptors = 1;
		heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		heapDesc.NodeMask = 0;
		ThrowIfFailed(mSRVHeap.Create(&heapDesc, mDevice.GetAddressOf()));
	}

	// DepthStencilView
	{
		D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
		heapDesc.NumDescriptors = 1;
		heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		heapDesc.NodeMask = 0;
		ThrowIfFailed(mDSVHeap.Create(&heapDesc, mDevice.GetAddressOf()));
	}
}

/// <summary>
/// ルートシグネチャの設定
/// </summary>
void Renderer::loadRootSignature()
{
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};

	// フラグ設定
	D3D12_ROOT_SIGNATURE_FLAGS flags;
	{
		flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	}
	rootSignatureDesc.Flags = flags;

	// ルートパラメタの設定
	D3D12_ROOT_PARAMETER rootParameters[2]{};
	{
		// ルートパラメータ[0] 
		{
			D3D12_ROOT_PARAMETER& parameter = rootParameters[0];
			parameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			parameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

			D3D12_DESCRIPTOR_RANGE ranges[2]{};
			{
				ranges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
				ranges[0].NumDescriptors = 1;
				ranges[0].BaseShaderRegister = 0;
				ranges[0].RegisterSpace = 0;
				ranges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

				ranges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
				ranges[1].NumDescriptors = 1;
				ranges[1].BaseShaderRegister = 1;
				ranges[1].RegisterSpace = 0;
				ranges[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
			}

			parameter.Descriptor.RegisterSpace = 0;
			parameter.Descriptor.ShaderRegister = 0;
			parameter.DescriptorTable.pDescriptorRanges = &ranges[0];
			parameter.DescriptorTable.NumDescriptorRanges = _countof(ranges);
		}

		// ルートパラメータ[1]
		{
			D3D12_ROOT_PARAMETER& parameter = rootParameters[1];
			parameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			parameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

			D3D12_DESCRIPTOR_RANGE ranges[1]{};
			{
				ranges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
				ranges[0].NumDescriptors = 1;
				ranges[0].BaseShaderRegister = 0;
				ranges[0].RegisterSpace = 0;
				ranges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
			}

			parameter.Descriptor.RegisterSpace = 0;
			parameter.Descriptor.ShaderRegister = 0;
			parameter.DescriptorTable.pDescriptorRanges = &ranges[0];
			parameter.DescriptorTable.NumDescriptorRanges = _countof(ranges);
		}

	}
	rootSignatureDesc.pParameters = rootParameters;
	rootSignatureDesc.NumParameters = _countof(rootParameters);

	// 静的サンプラーの設定
	D3D12_STATIC_SAMPLER_DESC rootSamplers[1]{};
	{
		// 静的サンプラー[0]
		{
			D3D12_STATIC_SAMPLER_DESC& sampler = rootSamplers[0];
			sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
			sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
			sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
			sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
			sampler.MipLODBias = 0.0f;
			sampler.MaxAnisotropy = 16;
			sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
			sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
			sampler.MinLOD = 0.0f;
			sampler.MaxLOD = D3D12_FLOAT32_MAX;
			sampler.ShaderRegister = 0;
			sampler.RegisterSpace = 0;
			sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		}
	}
	rootSignatureDesc.pStaticSamplers = &rootSamplers[0];
	rootSignatureDesc.NumStaticSamplers = _countof(rootSamplers);

	ComPtr<ID3DBlob> error;
	ComPtr<ID3DBlob> signature;
	ThrowIfFailed(D3D12SerializeRootSignature(
		&rootSignatureDesc,
		D3D_ROOT_SIGNATURE_VERSION_1,
		&signature,
		&error)
	);
	ThrowIfFailed(mDevice->CreateRootSignature(
		0,
		signature->GetBufferPointer(),
		signature->GetBufferSize(),
		IID_PPV_ARGS(&mRootSignature)
	));
}

/// <summary>
/// パイプラインステートを設定
/// </summary>
void Renderer::loadPipelineState()
{
	ComPtr<ID3DBlob> error;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{};
	psoDesc.pRootSignature = mRootSignature.Get();
	psoDesc.NodeMask = 0;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.SampleDesc.Count = 1;
	psoDesc.SampleDesc.Quality = 0;

	// 頂点インプットレイアウト
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
	{
		{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0,	 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL",		0, DXGI_FORMAT_R32G32B32_FLOAT,		0,	12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,		0,	24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0,	32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};
	psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };

#if defined(_DEBUG)
	// Enable better shader debugging with the graphics debugging tools.
	UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	UINT compileFlags = 0;
#endif

	/*
	{
		FILE* file = nullptr;
		fopen_s(&file, "VertexShader.cso", "rb");
		long int fsize = _filelength(_fileno(file));

		unsigned char* buffer = new unsigned char[fsize];
		fread_s(buffer, fsize, fsize, 1, file);
		fclose(file);

		D3D12_SHADER_BYTECODE byteCode;
		byteCode.pShaderBytecode = buffer;
		byteCode.BytecodeLength = fsize;
	}
	*/

	// 頂点シェーダ
	ComPtr<ID3DBlob> VS;
	ThrowIfFailed(D3DCompileFromFile(
		Application::getAssetFullPath(L"shaders.hlsl").c_str(),
		nullptr,
		nullptr,
		"VSMain",
		"vs_5_0",
		compileFlags,
		0,
		&VS,
		&error
	));
	psoDesc.VS = { VS->GetBufferPointer(), VS->GetBufferSize() };

	// ピクセルシェーダ
	ComPtr<ID3DBlob> PS;
	ThrowIfFailed(D3DCompileFromFile(
		Application::getAssetFullPath(L"shaders.hlsl").c_str(),
		nullptr,
		nullptr,
		"PSMain",
		"ps_5_0",
		compileFlags,
		0,
		&PS,
		&error
	));
	psoDesc.PS = { PS->GetBufferPointer(), PS->GetBufferSize() };

	// ブレンドステート
	D3D12_BLEND_DESC blendState;
	{
		D3D12_RENDER_TARGET_BLEND_DESC renderTarget =
		{
			FALSE, FALSE,
			D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
			D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
			D3D12_LOGIC_OP_NOOP,
			D3D12_COLOR_WRITE_ENABLE_ALL
		};
		for (UINT i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
		{
			blendState.RenderTarget[i] = renderTarget;
		}
		blendState.AlphaToCoverageEnable = FALSE;
		blendState.IndependentBlendEnable = FALSE;
	}
	psoDesc.BlendState = blendState;

	// ラスタライザー
	D3D12_RASTERIZER_DESC rasterizerState;
	{
		rasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
		rasterizerState.CullMode = D3D12_CULL_MODE_BACK;
		rasterizerState.FrontCounterClockwise = FALSE;
		rasterizerState.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
		rasterizerState.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
		rasterizerState.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
		rasterizerState.DepthClipEnable = TRUE;
		rasterizerState.MultisampleEnable = FALSE;
		rasterizerState.AntialiasedLineEnable = FALSE;
		rasterizerState.ForcedSampleCount = 0;
		rasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
	}
	psoDesc.RasterizerState = rasterizerState;

	// 深度ステンシル
	D3D12_DEPTH_STENCIL_DESC depthStencilState;
	{
		depthStencilState.DepthEnable = TRUE;
		depthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		depthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
		depthStencilState.StencilEnable = FALSE;
		depthStencilState.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
		depthStencilState.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;

		D3D12_DEPTH_STENCILOP_DESC frontFace;
		frontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
		frontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
		frontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
		frontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		depthStencilState.FrontFace = frontFace;

		D3D12_DEPTH_STENCILOP_DESC backFace;
		backFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
		backFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
		backFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
		backFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		depthStencilState.BackFace = backFace;
	}
	psoDesc.DepthStencilState = depthStencilState;

	ThrowIfFailed(mDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&mPSOGeometory)));
}

/// <summary>
/// ディスクリプタにリソース設定
/// </summary>
void Renderer::setDescriptorResource()
{
	// ディスクリプタヒープ : RTV
	{
		D3D12_RENDER_TARGET_VIEW_DESC resDesc{};

		for (UINT n = 0; n < FrameCount; n++)
		{
			mDevice->CreateRenderTargetView(mRenderTargets[n].Get(), nullptr, mRTVHeap.GetCPUDescriptorHandle(n));
		}
	}

	// ディスクリプタヒープ : DSV
	{
		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
		dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
		dsvDesc.Texture2D.MipSlice = 0;
		dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

		mDevice->CreateDepthStencilView(mDepthStencil.Get(), &dsvDesc, mDSVHeap.GetCPUDescriptorHandle(0));
	}

	// ディスクリプタヒープ : CBV
	{
		// ディスクリプタ[0] : DefaultConstantBuffer
		{
			D3D12_CONSTANT_BUFFER_VIEW_DESC bufferDesc{};
			bufferDesc.BufferLocation = mObjectConstantBuffer->GetGPUVirtualAddress();
			bufferDesc.SizeInBytes = sizeof(ObjectConstantBuffer);
			mDevice->CreateConstantBufferView(&bufferDesc, mCBVHeap.GetCPUDescriptorHandle(0));
		}

		// ディスクリプタ[1] : SceneConstantBuffer
		{
			D3D12_CONSTANT_BUFFER_VIEW_DESC bufferDesc{};
			bufferDesc.BufferLocation = mSceneConstantBuffer->GetGPUVirtualAddress();
			bufferDesc.SizeInBytes = sizeof(CameraConstantBuffer);
			mDevice->CreateConstantBufferView(&bufferDesc, mCBVHeap.GetCPUDescriptorHandle(1));
		}
	}
}

/// <summary>
/// リソースとデータポインタを設定
/// </summary>
void Renderer::setResourceDataPtr()
{
	// DefaultConstantBuffer
	{
		D3D12_RANGE readRange;
		readRange.Begin = 0;
		readRange.End = 0;

		ThrowIfFailed(mObjectConstantBuffer->Map(0, &readRange, reinterpret_cast<void**>(&mDataPtr[0])));
	}

	// SceneConstantBuffer
	{
		D3D12_RANGE readRange;
		readRange.Begin = 0;
		readRange.End = 0;

		ThrowIfFailed(mSceneConstantBuffer->Map(0, &readRange, reinterpret_cast<void**>(&mDataPtr[1])));
	}
}

void Renderer::createCommandQueue()
{
	// Describe and create the command queue.
	D3D12_COMMAND_QUEUE_DESC queueDesc{};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Priority = 0;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.NodeMask = 0;

	//	CommandQueue
	//	GPUに対して命令を発行する
	ThrowIfFailed(mDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&mCommandQueue)));
}

void Renderer::createSwapChain(IDXGIFactory4* factory)
{
	RECT rect;
	GetWindowRect(Application::getHwnd(), &rect);

	// Describe and create the swap chain.
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc1{};
	swapChainDesc1.Width = rect.right - rect.left;
	swapChainDesc1.Height = rect.bottom - rect.top;
	swapChainDesc1.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc1.Stereo = FALSE;
	swapChainDesc1.SampleDesc.Count = 1;
	swapChainDesc1.SampleDesc.Quality = 0;
	swapChainDesc1.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;		// フレームバッファの使用方法
	swapChainDesc1.BufferCount = FrameCount;							// バックバッファ数
	swapChainDesc1.Scaling = DXGI_SCALING_STRETCH;
	swapChainDesc1.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc1.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	swapChainDesc1.Flags = 0;

	//	FullScreen
	//	フルスクリーンモードを使用する際に設定
	DXGI_SWAP_CHAIN_FULLSCREEN_DESC swapChainFullScreenDesc{};
	swapChainFullScreenDesc.RefreshRate.Denominator = 1;
	swapChainFullScreenDesc.RefreshRate.Numerator = 60;
	swapChainFullScreenDesc.Scaling = DXGI_MODE_SCALING_STRETCHED;
	swapChainFullScreenDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainFullScreenDesc.Windowed = TRUE;

	//	SwapChain
	//	描画完了の表示待ちとなっているバックバッファを表画面と切り替える機構
	ComPtr<IDXGISwapChain1> swapChain;
	ThrowIfFailed(factory->CreateSwapChainForHwnd(
		mCommandQueue.Get(),		// Swap chain needs the queue so that it can force a flush on it.
		Application::getHwnd(),
		&swapChainDesc1,
		nullptr,
		nullptr,
		&swapChain
	));

	// This App does not support fullscreen transitions.
	ThrowIfFailed(factory->MakeWindowAssociation(Application::getHwnd(), DXGI_MWA_NO_ALT_ENTER));

	ThrowIfFailed(swapChain.As(&mSwapChain));

	mFrameIndex = mSwapChain->GetCurrentBackBufferIndex();
	mSwapChainEvent = mSwapChain->GetFrameLatencyWaitableObject();
}

void Renderer::createCommandAllocator()
{
	for (UINT n = 0; n < FrameCount; n++)
	{
		ThrowIfFailed(mDevice->CreateCommandAllocator(
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			IID_PPV_ARGS(&mCommandAllocators[n])
		));
	}
}

void Renderer::createCommandList()
{
	ThrowIfFailed(mDevice->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		mCommandAllocators[mFrameIndex].Get(),
		mPSOGeometory.Get(),
		IID_PPV_ARGS(&mCommandList)
	));

	// Command lists are created in the recording state, but there is nothing
	// to record yet. The main loop expects it to be closed, so close it now,
	ThrowIfFailed(mCommandList->Close());
}

void Renderer::createSyncObject()
{
	for (UINT i = 0; i < FrameCount; ++i) {
		ThrowIfFailed(mDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mFences[i])));
		mFenceValues[i] = 0;
	}
	++mFenceValues[mFrameIndex];

	// Create an event handle to use for frame synchronization.
	mFenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (mFenceEvent == nullptr)
	{
		ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
	}

	// GPUからFenceの値を変更する
	ThrowIfFailed(mCommandQueue->Signal(mFences[mFrameIndex].Get(), mFenceValues[mFrameIndex]));

	// 値がセットされるまでCPU待機
	ThrowIfFailed(mFences[mFrameIndex]->SetEventOnCompletion(mFenceValues[mFrameIndex], mFenceEvent));

	WaitForSingleObject(mFenceEvent, INFINITE);
}

void Renderer::createPipelineAssets()
{
	LONG width, height;
	RECT rect;
	GetWindowRect(Application::getHwnd(), &rect);

	width = rect.right - rect.left;
	height = rect.bottom - rect.top;

	// リソースの生成 : RenderTarget
	{
		for (UINT n = 0; n < FrameCount; n++)
		{
			ThrowIfFailed(mSwapChain->GetBuffer(n, IID_PPV_ARGS(&mRenderTargets[n])));
		}
	}

	// リソースの生成 : DepthStencil
	{
		D3D12_HEAP_PROPERTIES heapProp{};
		heapProp.Type = D3D12_HEAP_TYPE_DEFAULT;
		heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapProp.CreationNodeMask = 0;
		heapProp.VisibleNodeMask = 0;

		D3D12_RESOURCE_DESC resDesc{};
		resDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		resDesc.Alignment = 0;
		resDesc.Width = width;
		resDesc.Height = height;
		resDesc.DepthOrArraySize = 1;
		resDesc.MipLevels = 0;
		resDesc.Format = DXGI_FORMAT_D32_FLOAT;
		resDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		resDesc.SampleDesc.Count = 1;
		resDesc.SampleDesc.Quality = 0;
		resDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		D3D12_CLEAR_VALUE clearValue{};
		clearValue.Format = DXGI_FORMAT_D32_FLOAT;
		clearValue.DepthStencil.Depth = 1.0f;
		clearValue.DepthStencil.Stencil = 0;

		for (UINT n = 0; n < FrameCount; n++) {
			ThrowIfFailed(mDevice->CreateCommittedResource(
				&heapProp,
				D3D12_HEAP_FLAG_NONE,
				&resDesc,
				D3D12_RESOURCE_STATE_DEPTH_WRITE,
				&clearValue,
				IID_PPV_ARGS(&mDepthStencil))
			);
		}
	}

	// リソースの生成 : DefaultConstantBuffer
	{
		D3D12_HEAP_PROPERTIES heapProp{};
		heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
		heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapProp.CreationNodeMask = 0;
		heapProp.VisibleNodeMask = 0;

		D3D12_RESOURCE_DESC resDesc{};
		resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resDesc.Alignment = 0;
		resDesc.Width = sizeof(ObjectConstantBuffer);
		resDesc.Height = 1;
		resDesc.DepthOrArraySize = 1;
		resDesc.MipLevels = 1;
		resDesc.Format = DXGI_FORMAT_UNKNOWN;
		resDesc.SampleDesc.Count = 1;
		resDesc.SampleDesc.Quality = 0;
		resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

		ThrowIfFailed(mDevice->CreateCommittedResource(
			&heapProp,
			D3D12_HEAP_FLAG_NONE,
			&resDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&mObjectConstantBuffer))
		);
	}

	// リソースの生成 : SceneConstantBuffer
	{
		D3D12_HEAP_PROPERTIES heapProp{};
		heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
		heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapProp.CreationNodeMask = 0;
		heapProp.VisibleNodeMask = 0;

		D3D12_RESOURCE_DESC resDesc{};
		resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resDesc.Alignment = 0;
		resDesc.Width = sizeof(CameraConstantBuffer);
		resDesc.Height = 1;
		resDesc.DepthOrArraySize = 1;
		resDesc.MipLevels = 1;
		resDesc.Format = DXGI_FORMAT_UNKNOWN;
		resDesc.SampleDesc.Count = 1;
		resDesc.SampleDesc.Quality = 0;
		resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

		ThrowIfFailed(mDevice->CreateCommittedResource(
			&heapProp,
			D3D12_HEAP_FLAG_NONE,
			&resDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&mSceneConstantBuffer))
		);
	}
}

void Renderer::createAssets()
{
	// 頂点バッファの作成
	{
		// Define the geometry for a triangle.
		Vertex3D triangleVertices[] =
		{
			{ { -1.0f,  1.0f,  0.0f }, { 0.0f, 1.0f, 0.0f},{ 0.0f, 0.0f}, { 1.0f, 0.0f, 0.0f, 1.0f } },
			{ {  1.0f,  1.0f,  0.0f }, { 0.0f, 1.0f, 0.0f},{ 1.0f, 0.0f}, { 0.0f, 1.0f, 0.0f, 1.0f } },
			{ { -1.0f, -1.0f,  0.0f }, { 0.0f, 1.0f, 0.0f},{ 0.0f, 1.0f}, { 0.0f, 0.0f, 1.0f, 1.0f } },
			{ {  1.0f, -1.0f,  0.0f }, { 0.0f, 1.0f, 0.0f},{ 1.0f, 1.0f}, { 0.0f, 0.0f, 0.0f, 1.0f } }
		};
		const UINT vertexBufferSize = sizeof(triangleVertices);

		D3D12_HEAP_PROPERTIES heapProp{};
		heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
		heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapProp.CreationNodeMask = 0;
		heapProp.VisibleNodeMask = 0;

		D3D12_RESOURCE_DESC resDesc{};
		resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resDesc.Alignment = 0;
		resDesc.Width = vertexBufferSize;
		resDesc.Height = 1;
		resDesc.DepthOrArraySize = 1;
		resDesc.MipLevels = 1;
		resDesc.Format = DXGI_FORMAT_UNKNOWN;
		resDesc.SampleDesc.Count = 1;
		resDesc.SampleDesc.Quality = 0;
		resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

		ThrowIfFailed(mDevice->CreateCommittedResource(
			&heapProp,
			D3D12_HEAP_FLAG_NONE,
			&resDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&mVertexBuffer))
		);

		// Copy the triangle data to the vertex buffer.
		UINT8* pVertexDataBegin;

		// We do not intend to read from this resource on the CPU.
		D3D12_RANGE readRange;
		readRange.Begin = 0;
		readRange.End = 0;

		ThrowIfFailed(mVertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin)));
		memcpy(pVertexDataBegin, triangleVertices, sizeof(triangleVertices));
		mVertexBuffer->Unmap(0, nullptr);

		// Initialize the vertex buffer view.
		mVertexBufferView.BufferLocation = mVertexBuffer->GetGPUVirtualAddress();
		mVertexBufferView.StrideInBytes = sizeof(Vertex3D);
		mVertexBufferView.SizeInBytes = vertexBufferSize;
	}

	// インベックスバッファの作成
	{
		UINT32 indices[] =
		{
			0,1,2,
			1,3,2
		};
		const UINT indexBufferSize = sizeof(indices);

		D3D12_HEAP_PROPERTIES heapProp{};
		heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
		heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapProp.CreationNodeMask = 0;
		heapProp.VisibleNodeMask = 0;

		D3D12_RESOURCE_DESC resDesc{};
		resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resDesc.Alignment = 0;
		resDesc.Width = indexBufferSize;
		resDesc.Height = 1;
		resDesc.DepthOrArraySize = 1;
		resDesc.MipLevels = 1;
		resDesc.Format = DXGI_FORMAT_UNKNOWN;
		resDesc.SampleDesc.Count = 1;
		resDesc.SampleDesc.Quality = 0;
		resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

		ThrowIfFailed(mDevice->CreateCommittedResource(
			&heapProp,
			D3D12_HEAP_FLAG_NONE,
			&resDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&mIndexBuffer))
		);

		UINT8* pIndexDataBegin;

		D3D12_RANGE readRange;
		readRange.Begin = 0;
		readRange.End = 0;

		ThrowIfFailed(mIndexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pIndexDataBegin)));
		memcpy(pIndexDataBegin, indices, indexBufferSize);
		mIndexBuffer->Unmap(0, nullptr);

		mIndexBufferView.BufferLocation = mIndexBuffer->GetGPUVirtualAddress();
		mIndexBufferView.SizeInBytes = indexBufferSize;
		mIndexBufferView.Format = DXGI_FORMAT_R32_UINT;

		mIndexCount = _countof(indices);
	}

	// バンドル生成
	{
		// Create the Bundle Allocator
		ThrowIfFailed(mDevice->CreateCommandAllocator(
			D3D12_COMMAND_LIST_TYPE_BUNDLE,
			IID_PPV_ARGS(&mBundleAllocator))
		);

		// Create and record the bundle
		{
			ThrowIfFailed(mDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_BUNDLE, mBundleAllocator.Get(), mPSOGeometory.Get(), IID_PPV_ARGS(&mBundle)));
			mBundle->SetGraphicsRootSignature(mRootSignature.Get());
			mBundle->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			mBundle->IASetVertexBuffers(0, 1, &mVertexBufferView);
			mBundle->IASetIndexBuffer(&mIndexBufferView);
			mBundle->DrawIndexedInstanced(mIndexCount, 1, 0, 0, 0);
			ThrowIfFailed(mBundle->Close());
		}
	}
}

void Renderer::begin()
{
	resetCommandList(mCommandAllocators[mFrameIndex].Get());

#if 1
	{
		D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(mRenderTargets[mFrameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		mCommandList->ResourceBarrier(1, &barrier);
	}
#else
	// Indicate that the back buffer will be used as a render target.
	{
		// スワップチェイン表示可能状態(D3D12_RESOURCE_STATE_PRESENT) から レンダーターゲット状態(D3D12_RESOURCE_STATE_RENDER_TARGET)へ

		D3D12_RESOURCE_BARRIER barrier;
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = mRenderTargets[mFrameIndex].Get();
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

		mCommandList->ResourceBarrier(1, &barrier);
	}
#endif
}

void Renderer::record(Camera* pCamera)
{
	// Signatureを設定
	mCommandList->SetGraphicsRootSignature(mRootSignature.Get());

	if (pCamera != nullptr)
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(mRTVHeap.GetCPUDescriptorHandle(mFrameIndex));
		CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(mDSVHeap.GetCPUDescriptorHandle(0));

		// RS : Rasterizer
		// ビューポートとシザー矩形の設定
		mCommandList->RSSetViewports(pCamera->getNumViewport(), &pCamera->getViewport());
		mCommandList->RSSetScissorRects(pCamera->getNumViewport(), &pCamera->getScissorRect());

		mCommandList->ClearRenderTargetView(rtvHandle, pCamera->getClearColor(), 0, nullptr);
		mCommandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

		// RootParameterIndex
		// Signatureに設定したパラメータに紐づける
		ID3D12DescriptorHeap* ppHeaps[] = { mCBVHeap.GetHeap() };
		mCommandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

		// SetConstantBuffer
		mCommandList->SetGraphicsRootDescriptorTable(0, mCBVHeap.GetGPUDescriptorHandle(0));

		// 対応するパラメータタイプがD3D12_ROOT_PARAMETER_TYPE_CBVの場合
		//mCommandList->SetGraphicsRootConstantBufferView(0,mConstantBuffer[0]->GetGPUVirtualAddress() + sizeof(ConstantBuffer) * mFrameIndex);

		// OM : Output Merger
		mCommandList->OMSetRenderTargets(1, &rtvHandle, TRUE, &dsvHandle);

		// オブジェクト描画
		PIXBeginEvent(mCommandList.Get(), 0, L"Draw Object");
		{
			// Use Bundle
			//mCommandList->ExecuteBundle(mBundle.Get());

			// IA : Input Assember
			mCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			mCommandList->IASetVertexBuffers(0, 1, &mVertexBufferView);
			mCommandList->IASetIndexBuffer(&mIndexBufferView);

			mCommandList->DrawIndexedInstanced(mIndexCount, 1, 0, 0, 0);
		}
		PIXEndEvent(mCommandList.Get());
	}
}

void Renderer::end()
{
#if 1
	{
		D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(mRenderTargets[mFrameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		mCommandList->ResourceBarrier(1, &barrier);
	}
#else
	// Indicate that the back buffer will now be used to present.
	{
		// レンダーターゲット状態(D3D12_RESOURCE_STATE_RENDER_TARGET) から スワップチェイン表示可能状態(D3D12_RESOURCE_STATE_PRESENT) へ

		D3D12_RESOURCE_BARRIER barrier;
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = mRenderTargets[mFrameIndex].Get();
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

		mCommandList->ResourceBarrier(1, &barrier);
}
#endif

	ThrowIfFailed(mCommandList->Close());
}

void Renderer::resetCommandList(ID3D12CommandAllocator* const allocator)
{
	// Command list allocators can only be reset when the associated 
	// command lists have finished execution on the GPU; apps should use 
	// fences to determine GPU execution progress.
	ThrowIfFailed(allocator->Reset());

	// However, when ExecuteCommandList() is called on a particular command 
	// list, that command list can then be reset at any time and must be before 
	// re-recording.
	ThrowIfFailed(mCommandList->Reset(allocator, mPSOGeometory.Get()));
}

void Renderer::populateCommandList()
{

}

void Renderer::waitForGpu()
{
	// GPUからFenceの値を変更する
	ThrowIfFailed(mCommandQueue->Signal(mFences[mFrameIndex].Get(), mFenceValues[mFrameIndex]));

	// 値がセットされるまでCPU待機
	ThrowIfFailed(mFences[mFrameIndex]->SetEventOnCompletion(mFenceValues[mFrameIndex], mFenceEvent));
	WaitForSingleObject(mFenceEvent, INFINITE);

	++mFenceValues[mFrameIndex];
}

void Renderer::moveToNextFrame()
{
	const UINT64 currentFenceValue = mFenceValues[mFrameIndex];
	ThrowIfFailed(mCommandQueue->Signal(mFences[mFrameIndex].Get(), currentFenceValue));

	// Update the frame index.
	mFrameIndex = mSwapChain->GetCurrentBackBufferIndex();

	// If the next frame is not ready to be rendered yet, wait until it is ready.
	if (mFences[mFrameIndex]->GetCompletedValue() < mFenceValues[mFrameIndex])
	{
		ThrowIfFailed(mFences[mFrameIndex]->SetEventOnCompletion(mFenceValues[mFrameIndex], mFenceEvent));
		WaitForSingleObjectEx(mFenceEvent, INFINITE, FALSE);
	}

	mFenceValues[mFrameIndex] = currentFenceValue + 1;
}

#if defined(_DEBUG)
void Renderer::enableDebugLayer(UINT& dxgiFactoryFlags)
{
	// Enable the debug layer (requires the Graphics Tools "optional feature").
	// NOTE: Enabling the debug layer after device creation will invalidate the active device.
	ComPtr<ID3D12Debug> debug;
	if (FAILED(D3D12GetDebugInterface(IID_PPV_ARGS(&debug)))) return;
	debug->EnableDebugLayer();

	// Enable additional debug layers.
	dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;

	// Enable the GPUBasedValidation
	ComPtr<ID3D12Debug3> debug3;
	debug.As(&debug3);
	debug3->SetEnableGPUBasedValidation(false);
}
#endif

HRESULT DescriptorHeap::Create(const D3D12_DESCRIPTOR_HEAP_DESC* pDesc, ID3D12Device** const ppDevice)
{
	HRESULT hr = (*ppDevice)->CreateDescriptorHeap(pDesc, IID_PPV_ARGS(&mHeap));
	if (SUCCEEDED(hr))
	{
		mSize = (*ppDevice)->GetDescriptorHandleIncrementSize(pDesc->Type);
	}
	return hr;
}

D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeap::GetCPUDescriptorHandle(UINT offset)
{
	D3D12_CPU_DESCRIPTOR_HANDLE handle = mHeap->GetCPUDescriptorHandleForHeapStart();
	handle.ptr += mSize * offset;
	return handle;
}

D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeap::GetGPUDescriptorHandle(UINT offset)
{
	D3D12_GPU_DESCRIPTOR_HANDLE handle = mHeap->GetGPUDescriptorHandleForHeapStart();
	handle.ptr += mSize * offset;
	return handle;
}

void Plane::onSetup()
{

}

void Plane::onUpdate()
{
	Quaternion rotation = getTransform()->getLocalRotation();
	rotation *= quaternion::AxisToRadian(mathf::PIDIV4 * 0.025f,vector3::ZAxis);
	getTransform()->setLocalRotation(rotation);
}

void Plane::onRender()
{
	ObjectConstantBuffer buffer;

	XMFLOAT4X4 matrix;
	XMStoreFloat4x4(&matrix, XMMatrixTranspose(getTransform()->getWorldMatrix()));
	buffer.world = matrix;

	Renderer::getInstance()->onRegisterDataBuffer(0, &buffer, sizeof(ObjectConstantBuffer));
}
