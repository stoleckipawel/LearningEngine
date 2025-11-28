#include "PCH.h"
#include "DX12/PSO.h"

// PSO.cpp
// Implements Pipeline State Object (PSO) setup and configuration for D3D12 rendering.

/**
 * @brief Configures stream output for the pipeline state (disabled by default).
 */
void PSO::SetStreamOutput(D3D12_GRAPHICS_PIPELINE_STATE_DESC& psoDesc)
{
	psoDesc.StreamOutput.NumEntries = 0;
	psoDesc.StreamOutput.pSODeclaration = nullptr;
	psoDesc.StreamOutput.NumStrides = 0;
	psoDesc.StreamOutput.pBufferStrides = nullptr;
	psoDesc.StreamOutput.RasterizedStream = 0;
}

// Configures rasterizer state for the pipeline.
void PSO::SetRasterizerState(D3D12_GRAPHICS_PIPELINE_STATE_DESC& psoDesc, bool bRenderWireframe, D3D12_CULL_MODE cullMode)
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

// Configures blend state for the render target.
void PSO::SetRenderTargetBlendState(D3D12_GRAPHICS_PIPELINE_STATE_DESC& psoDesc, D3D12_RENDER_TARGET_BLEND_DESC blendDesc)
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


// Configures depth test state for the pipeline.
void PSO::SetDepthTestState(D3D12_GRAPHICS_PIPELINE_STATE_DESC& psoDesc, D3D12_DEPTH_TEST_DESC depthDesc)
{
	psoDesc.DepthStencilState.DepthEnable = depthDesc.DepthEnable;
	psoDesc.DepthStencilState.DepthWriteMask = depthDesc.DepthWriteMask;
	psoDesc.DepthStencilState.DepthFunc = depthDesc.DepthFunc;
}


// Configures stencil test state for the pipeline.
void PSO::SetStencilTestState(D3D12_GRAPHICS_PIPELINE_STATE_DESC& psoDesc, D3D12_STENCIL_TEST_DESC stencilDesc)
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

// Constructs and creates the graphics pipeline state object (PSO)
PSO::PSO(Geometry& vertecies, ID3D12RootSignature* rootSignature, ShaderCompiler& vertexShader, ShaderCompiler& pixelShader)
{

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};

	// -- Vertex Data
	std::vector<D3D12_INPUT_ELEMENT_DESC> vertexLayout = vertecies.GetVertexLayout();
	psoDesc.InputLayout.NumElements = UINT(vertexLayout.size());
	psoDesc.InputLayout.pInputElementDescs = vertexLayout.data();
	psoDesc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;

	// -- Root Signature
	psoDesc.pRootSignature = rootSignature;

	// -- Vertex Shader
	psoDesc.VS.pShaderBytecode = vertexShader.GetBuffer();
	psoDesc.VS.BytecodeLength = vertexShader.GetSize();

	// -- Pixel Shader
	psoDesc.PS.pShaderBytecode = pixelShader.GetBuffer();
	psoDesc.PS.BytecodeLength = pixelShader.GetSize();

	// -- Rasterizer
	SetRasterizerState(psoDesc, false, D3D12_CULL_MODE_NONE);

	// -- Stream Output 
	SetStreamOutput(psoDesc);

	// -- Blend State
	psoDesc.BlendState.AlphaToCoverageEnable = false;
	psoDesc.BlendState.IndependentBlendEnable = false; // Multiple RenderTarget Varied Blending

	D3D12_RENDER_TARGET_BLEND_DESC renderTargetBlendDesc = {};
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
	SetRenderTargetBlendState(psoDesc, renderTargetBlendDesc);

	// -- Depth Test State
	D3D12_DEPTH_TEST_DESC depthTestDesc = {};
	depthTestDesc.DepthEnable = true;
	depthTestDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	depthTestDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	SetDepthTestState(psoDesc, depthTestDesc);

	// -- Stencil Test State
	D3D12_STENCIL_TEST_DESC stencilTestDesc = {};
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
	SetStencilTestState(psoDesc, stencilTestDesc);

	// -- Render Target and Depth Stencil Formats
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	// -- Miscellaneous
	psoDesc.NodeMask = 0;
	psoDesc.CachedPSO.CachedBlobSizeInBytes = 0;
	psoDesc.CachedPSO.pCachedBlob = nullptr;
	psoDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	psoDesc.SampleMask = 0xFFFFFFFF;
	psoDesc.SampleDesc.Count = 1;
	psoDesc.SampleDesc.Quality = 0;


	// -- Create PSO (manual HRESULT check and info queue logging)
	HRESULT hr = GRHI.Device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pso));
	if (FAILED(hr))
	{	
		// If debug layer is enabled, query InfoQueue for messages
		ComPointer<ID3D12InfoQueue> infoQueue;
		if (SUCCEEDED(GRHI.Device->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
			UINT64 numMessages = infoQueue->GetNumStoredMessages();
			for (UINT64 i = 0; i < numMessages; ++i) {
				SIZE_T messageLength = 0;
				infoQueue->GetMessage(i, nullptr, &messageLength);
				std::vector<char> messageData(messageLength);
				D3D12_MESSAGE* message = reinterpret_cast<D3D12_MESSAGE*>(messageData.data());
				infoQueue->GetMessage(i, message, &messageLength);
				LogMessage(std::string("D3D12 InfoQueue: ") + message->pDescription, ELogType::Info);
			}
		}
		char buf[512];
		std::snprintf(buf, sizeof(buf), "Failed To Create PSO. HRESULT: 0x%08X", static_cast<unsigned int>(hr));
		LogMessage(buf, ELogType::Fatal);
	}
}

void PSO::Release()
{
	m_pso.Release();
}

PSO::~PSO()
{
	Release();
}

// Sets the pipeline state object for the current command list.
void PSO::Set()
{
	GRHI.GetCommandList()->SetPipelineState(m_pso);
}