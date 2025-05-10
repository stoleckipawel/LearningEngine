#include "D3D12Renderer.h"
#include "D3D12DebugLayer.h"
#include "D3D12Context.h"
#include "D3D12Window.h"



bool D3D12Renderer::Initialize()
{
	if (FAILED(D3D12DebugLayer::Get().Initialize()))
	{
		return false;
	}

	if(FAILED(D3D12Context::Get().Initialize()))
	{
		return false;
	}
		
	if(FAILED(D3D12Window::Get().Initialize()))
	{
		return false;
	}
	D3D12Window::Get().SetFullScreen(false);


	return true;
}



void D3D12Renderer::Shutdown()
{
}

std::vector<D3D12_INPUT_ELEMENT_DESC> D3D12Renderer::GetVertexLayout()
{
	return
	{
		{ "Position",  0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TexCoord",  0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};
}

D3D12_RESOURCE_DESC D3D12Renderer::CreateVertexBufferDesc(uint32_t VertexCount)
{
	D3D12_RESOURCE_DESC vertexResourceDesc = {};
	vertexResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	vertexResourceDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
	vertexResourceDesc.Width = VertexCount;
	vertexResourceDesc.Height = 1;
	vertexResourceDesc.DepthOrArraySize = 1;
	vertexResourceDesc.MipLevels = 1;
	vertexResourceDesc.Format = DXGI_FORMAT_UNKNOWN;
	vertexResourceDesc.SampleDesc.Count = 1;
	vertexResourceDesc.SampleDesc.Quality = 0;
	vertexResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	vertexResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	return vertexResourceDesc;
}

D3D12_RESOURCE_DESC D3D12Renderer::CreateTextureResourceDesc(D3D12ImageLoader::ImageData& textureData)
{
	D3D12_RESOURCE_DESC textureDesc{};
	textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	textureDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
	textureDesc.Width = textureData.width;
	textureDesc.Height = textureData.height;
	textureDesc.DepthOrArraySize = 1;
	textureDesc.MipLevels = 1;//#ToDo compute mip count
	textureDesc.Format = textureData.dxgiPixelFormat;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	textureDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	return textureDesc;
}

void D3D12Renderer::SetStreamOutput(D3D12_GRAPHICS_PIPELINE_STATE_DESC& psoDesc)
{
	psoDesc.StreamOutput.NumEntries = 0;
	psoDesc.StreamOutput.pSODeclaration = nullptr;
	psoDesc.StreamOutput.NumStrides = 0;
	psoDesc.StreamOutput.pBufferStrides = nullptr;
	psoDesc.StreamOutput.RasterizedStream = 0;
}

void D3D12Renderer::SetRasterizerState(D3D12_GRAPHICS_PIPELINE_STATE_DESC& psoDesc, bool bRenderWireframe, D3D12_CULL_MODE cullMode)
{
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.RasterizerState.FillMode = bRenderWireframe ? D3D12_FILL_MODE_WIREFRAME : D3D12_FILL_MODE_SOLID;
	psoDesc.RasterizerState.CullMode = cullMode;
	psoDesc.RasterizerState.FrontCounterClockwise = false;
	psoDesc.RasterizerState.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
	psoDesc.RasterizerState.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
	psoDesc.RasterizerState.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
	psoDesc.RasterizerState.DepthClipEnable = true;
	psoDesc.RasterizerState.MultisampleEnable = false;
	psoDesc.RasterizerState.AntialiasedLineEnable = false;
	psoDesc.RasterizerState.ForcedSampleCount = 0;
	psoDesc.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
}

void D3D12Renderer::SetRenderTargetBlendState(D3D12_GRAPHICS_PIPELINE_STATE_DESC& psoDesc, D3D12_RENDER_TARGET_BLEND_DESC blendDesc)
{
	psoDesc.BlendState.RenderTarget[0].BlendEnable = blendDesc.BlendEnable;
	psoDesc.BlendState.RenderTarget[0].BlendOp = blendDesc.BlendOp;
	psoDesc.BlendState.RenderTarget[0].BlendOpAlpha = blendDesc.BlendOpAlpha;
	psoDesc.BlendState.RenderTarget[0].SrcBlend = blendDesc.SrcBlend;
	psoDesc.BlendState.RenderTarget[0].DestBlend = blendDesc.DestBlend;
	psoDesc.BlendState.RenderTarget[0].SrcBlendAlpha = blendDesc.SrcBlendAlpha;
	psoDesc.BlendState.RenderTarget[0].DestBlendAlpha = blendDesc.DestBlendAlpha;
	psoDesc.BlendState.RenderTarget[0].LogicOp = blendDesc.LogicOp;
	psoDesc.BlendState.RenderTarget[0].LogicOpEnable = blendDesc.LogicOpEnable;
	psoDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = blendDesc.RenderTargetWriteMask;
}

void D3D12Renderer::SetDepthTestState(D3D12_GRAPHICS_PIPELINE_STATE_DESC& psoDesc, D3D12_DEPTH_TEST_DESC depthDesc)
{
	psoDesc.DepthStencilState.DepthEnable = depthDesc.DepthEnable;
	psoDesc.DepthStencilState.DepthWriteMask = depthDesc.DepthWriteMask;
	psoDesc.DepthStencilState.DepthFunc = depthDesc.DepthFunc;
}

void D3D12Renderer::SetStencilTestState(D3D12_GRAPHICS_PIPELINE_STATE_DESC& psoDesc, D3D12_STENCIL_TEST_DESC stencilDesc)
{
	psoDesc.DepthStencilState.StencilEnable = stencilDesc.StencilEnable;
	psoDesc.DepthStencilState.StencilReadMask = stencilDesc.StencilReadMask;
	psoDesc.DepthStencilState.StencilWriteMask = stencilDesc.StencilWriteMask;
	psoDesc.DepthStencilState.FrontFace.StencilFunc = stencilDesc.FrontFaceStencilFunc;
	psoDesc.DepthStencilState.FrontFace.StencilFailOp = stencilDesc.FrontFaceStencilFailOp;
	psoDesc.DepthStencilState.FrontFace.StencilDepthFailOp = stencilDesc.FrontFaceStencilDepthFailOp;
	psoDesc.DepthStencilState.FrontFace.StencilPassOp = stencilDesc.FrontFaceStencilPassOp;
	psoDesc.DepthStencilState.BackFace.StencilFunc = stencilDesc.BackFaceStencilFunc;
	psoDesc.DepthStencilState.BackFace.StencilFailOp = stencilDesc.BackFaceStencilFailOp;
	psoDesc.DepthStencilState.BackFace.StencilDepthFailOp = stencilDesc.BackFaceStencilDepthFailOp;
	psoDesc.DepthStencilState.BackFace.StencilPassOp = stencilDesc.BackFaceStencilPassOp;
}


void UploadBuffer(ComPointer<ID3D12Resource2>& buffer, void* data, uint32_t dataSize)
{
	//Heaps
	D3D12_HEAP_PROPERTIES heapDefaultProperties = {};
	heapDefaultProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
	heapDefaultProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapDefaultProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapDefaultProperties.CreationNodeMask = 0;
	heapDefaultProperties.VisibleNodeMask = 0;

	D3D12_HEAP_PROPERTIES heapUploadProperties = {};
	heapUploadProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
	heapUploadProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapUploadProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapUploadProperties.CreationNodeMask = 0;
	heapUploadProperties.VisibleNodeMask = 0;

	//Resource Descs
	D3D12_RESOURCE_DESC resourceDesc = {};
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resourceDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
	resourceDesc.Width = dataSize;
	resourceDesc.Height = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.SampleDesc.Quality = 0;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	D3D12_RESOURCE_DESC uploadResourceDesc{};
	uploadResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	uploadResourceDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
	uploadResourceDesc.Width = dataSize;
	uploadResourceDesc.Height = 1;
	uploadResourceDesc.DepthOrArraySize = 1;
	uploadResourceDesc.MipLevels = 1;
	uploadResourceDesc.Format = DXGI_FORMAT_UNKNOWN;
	uploadResourceDesc.SampleDesc.Count = 1;
	uploadResourceDesc.SampleDesc.Quality = 0;
	uploadResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	uploadResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	//Create Commited Resources
	D3D12Context::Get().GetDevice()->CreateCommittedResource(
		&heapDefaultProperties, D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		IID_PPV_ARGS(&buffer));

	ComPointer<ID3D12Resource2> uploadBuffer;
	D3D12Context::Get().GetDevice()->CreateCommittedResource(
		&heapUploadProperties,
		D3D12_HEAP_FLAG_NONE,
		&uploadResourceDesc,
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		IID_PPV_ARGS(&uploadBuffer));

	//Copy data -> CPU RESOURCE
	char* uploadBufferAddress;
	D3D12_RANGE uploadRange;
	uploadRange.Begin = 0;
	uploadRange.End = dataSize;
	uploadBuffer->Map(0, &uploadRange, (void**)&uploadBufferAddress);
	memcpy(&uploadBufferAddress[0], data, dataSize);
	uploadBuffer->Unmap(0, &uploadRange);

	// Copy CPU Resource -> GPU Resource
	auto cmdList = D3D12Context::Get().InitializeCommandList();
	cmdList->CopyBufferRegion(buffer, 0, uploadBuffer, 0, dataSize);
	D3D12Context::Get().ExecuteCommandList();

	//uploadBuffer->Release();
}
void D3D12Renderer::UploadVertecies()
{
	Vertex verts[] =
	{
		//T1
		{-1.0, -1.0, 0.0, 0.0, 1.0},
		{0.0, 1.0, 0.0, 0.5, 0.0},
		{1.0, -1.0, 0.0, 1.0, 1.0}
	};

	uint32_t vertsDataSize = sizeof(Vertex) * _countof(verts);
	UploadBuffer(vertexBuffer, verts, vertsDataSize);

	vertexBufferView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
	vertexBufferView.SizeInBytes = vertsDataSize;
	vertexBufferView.StrideInBytes = sizeof(Vertex);




}

void D3D12Renderer::CreateRootSignature()
{
	Shader rootSignatureShader = Shader("RootSignature.cso");

	D3D12Context::Get().GetDevice()->CreateRootSignature(
		0,
		rootSignatureShader.GetBuffer(),
		rootSignatureShader.GetSize(),
		IID_PPV_ARGS(&rootSignature));
}


void D3D12Renderer::CreatePSO()
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};

	// -- Vertex Data
	UploadVertecies();
	std::vector<D3D12_INPUT_ELEMENT_DESC> vertexLayout = GetVertexLayout();
	psoDesc.InputLayout.NumElements = vertexLayout.size();
	psoDesc.InputLayout.pInputElementDescs = vertexLayout.data();
	psoDesc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;


	// -- Root Signature
	CreateRootSignature();
	psoDesc.pRootSignature = rootSignature;

	//VS	
	Shader vertexShader = Shader("VertexShader.cso");
	psoDesc.VS.pShaderBytecode = vertexShader.GetBuffer();
	psoDesc.VS.BytecodeLength = vertexShader.GetSize();
	//PS
	Shader pixelShader = Shader("PixelShader.cso");
	psoDesc.PS.pShaderBytecode = pixelShader.GetBuffer();
	psoDesc.PS.BytecodeLength = pixelShader.GetSize();

	// -- Rasterizer
	SetRasterizerState(psoDesc, false, D3D12_CULL_MODE_NONE);

	//StreamOutput
	SetStreamOutput(psoDesc);

	//Blend State
	psoDesc.BlendState.AlphaToCoverageEnable = false;
	psoDesc.BlendState.IndependentBlendEnable = false;//Multiple RenderTarget Varied Blending

	D3D12_RENDER_TARGET_BLEND_DESC renderTargetBlendDesc;
	{
		renderTargetBlendDesc.BlendEnable = false;
		renderTargetBlendDesc.BlendOp = D3D12_BLEND_OP_ADD;
		renderTargetBlendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
		renderTargetBlendDesc.SrcBlend = D3D12_BLEND_ONE;
		renderTargetBlendDesc.DestBlend = D3D12_BLEND_ZERO;
		renderTargetBlendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
		renderTargetBlendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;
		renderTargetBlendDesc.LogicOp = D3D12_LOGIC_OP_NOOP;
		renderTargetBlendDesc.LogicOpEnable = false;
		renderTargetBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	}
	SetRenderTargetBlendState(psoDesc, renderTargetBlendDesc);


	D3D12_DEPTH_TEST_DESC depthTestDesc;
	{
		depthTestDesc.DepthEnable = false;
		depthTestDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
		depthTestDesc.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	}
	SetDepthTestState(psoDesc, depthTestDesc);

	D3D12_STENCIL_TEST_DESC stencilTestDesc;
	{
		stencilTestDesc.StencilEnable = false;
		stencilTestDesc.StencilReadMask = 0;
		stencilTestDesc.StencilWriteMask = 0;
		stencilTestDesc.FrontFaceStencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		stencilTestDesc.FrontFaceStencilFailOp = D3D12_STENCIL_OP_KEEP;
		stencilTestDesc.FrontFaceStencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
		stencilTestDesc.FrontFaceStencilPassOp = D3D12_STENCIL_OP_KEEP;
		stencilTestDesc.BackFaceStencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		stencilTestDesc.BackFaceStencilFailOp = D3D12_STENCIL_OP_KEEP;
		stencilTestDesc.BackFaceStencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
		stencilTestDesc.BackFaceStencilPassOp = D3D12_STENCIL_OP_KEEP;
	}
	SetStencilTestState(psoDesc, stencilTestDesc);

	// -- NumRenderTargets
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.DSVFormat = DXGI_FORMAT_UNKNOWN;//DXGI_FORMAT_D24_UNORM_S8_UINT;

	//Misc
	psoDesc.NodeMask = 0;
	psoDesc.CachedPSO.CachedBlobSizeInBytes = 0;
	psoDesc.CachedPSO.pCachedBlob = nullptr;
	psoDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	psoDesc.SampleMask = 0xFFFFFFFF;
	psoDesc.SampleDesc.Count = 1;
	psoDesc.SampleDesc.Quality = 0;

	// -- Create PSO
	D3D12Context::Get().GetDevice()->CreateGraphicsPipelineState(
		&psoDesc,
		IID_PPV_ARGS(&pso));
}

void D3D12Renderer::SetPSO(ComPointer<ID3D12GraphicsCommandList7>& cmdList)
{
	//PSO
	cmdList->SetPipelineState(pso);
	cmdList->SetGraphicsRootSignature(rootSignature);
	//Input Assembler
	cmdList->IASetVertexBuffers(0, 1, &vertexBufferView);
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

}

void D3D12Renderer::SetViewport(ComPointer<ID3D12GraphicsCommandList7>& cmdList)
{
	//Rasterizer State: Viewport
	D3D12_VIEWPORT viewport = D3D12Window::Get().GetDefaultViewport();
	cmdList->RSSetViewports(1, &viewport);
	//Rasterizer State: Scissor  
	D3D12_RECT scissorRect = D3D12Window::Get().GetDefaultScissorRect();
	cmdList->RSSetScissorRects(1, &scissorRect);
}
