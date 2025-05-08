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

void D3D12Renderer::LoadAssets()
{

}

void D3D12Renderer::UploadVertecies()
{
	Vertex vertecies[] =
	{
		//T1
		{-1.0, -1.0, 0.0, 0.0, 1.0},
		{0.0, 1.0, 0.0, 0.5, 0.0},
		{1.0, -1.0, 0.0, 1.0, 1.0}
	};

	D3D12_HEAP_PROPERTIES heapDefaultProperties = {};
	heapDefaultProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
	heapDefaultProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapDefaultProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapDefaultProperties.CreationNodeMask = 0;
	heapDefaultProperties.VisibleNodeMask = 0;

	uint32_t verteciesDataSize = sizeof(Vertex) * _countof(vertecies);
	D3D12_RESOURCE_DESC vertexBufferDesc = D3D12Renderer::Get().CreateVertexBufferDesc(verteciesDataSize);
	D3D12Context::Get().GetDevice()->CreateCommittedResource(
		&heapDefaultProperties, D3D12_HEAP_FLAG_NONE,
		&vertexBufferDesc,
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		IID_PPV_ARGS(&vertexBuffer));

	vertexBufferView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
	vertexBufferView.SizeInBytes = verteciesDataSize;
	vertexBufferView.StrideInBytes = sizeof(Vertex);

	D3D12_HEAP_PROPERTIES heapUploadProperties = {};
	heapUploadProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
	heapUploadProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapUploadProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapUploadProperties.CreationNodeMask = 0;
	heapUploadProperties.VisibleNodeMask = 0;

	D3D12_RESOURCE_DESC uploadResourceDesc{};
	uploadResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	uploadResourceDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
	uploadResourceDesc.Width = verteciesDataSize;
	uploadResourceDesc.Height = 1;
	uploadResourceDesc.DepthOrArraySize = 1;
	uploadResourceDesc.MipLevels = 1;
	uploadResourceDesc.Format = DXGI_FORMAT_UNKNOWN;
	uploadResourceDesc.SampleDesc.Count = 1;
	uploadResourceDesc.SampleDesc.Quality = 0;
	uploadResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	uploadResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	D3D12Context::Get().GetDevice()->CreateCommittedResource(
		&heapUploadProperties,
		D3D12_HEAP_FLAG_NONE,
		&uploadResourceDesc,
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		IID_PPV_ARGS(&uploadBuffer));

	//Copy void** -> CPU RESOURCE
	char* uploadBufferAddress;
	D3D12_RANGE uploadRange;
	uploadRange.Begin = 0;
	uploadRange.End = verteciesDataSize;
	uploadBuffer->Map(0, &uploadRange, (void**)&uploadBufferAddress);
	memcpy(&uploadBufferAddress[0], vertecies, verteciesDataSize);
	uploadBuffer->Unmap(0, &uploadRange);

	// Copy CPU Resource -> GPU Resource
	auto cmdList = D3D12Context::Get().InitializeCommandList();
	cmdList->CopyBufferRegion(vertexBuffer, 0, uploadBuffer, 0, verteciesDataSize);
	D3D12Context::Get().ExecuteCommandList();

}

