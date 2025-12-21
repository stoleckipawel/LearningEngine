#include "PCH.h"
#include "PSO.h"
#include "DebugUtils.h"
#include "Error.h"

#include <vector>
#include <string>
#include <memory>

// PSO.cpp
// Implements Pipeline State Object (PSO) setup and configuration for D3D12 rendering.


// Configure stream output for the pipeline state (disabled by default).
void PSO::SetStreamOutput(D3D12_GRAPHICS_PIPELINE_STATE_DESC& psoDesc) noexcept
{
	psoDesc.StreamOutput = {};
}

// Configures rasterizer state for the pipeline.
void PSO::SetRasterizerState(D3D12_GRAPHICS_PIPELINE_STATE_DESC& psoDesc, bool bRenderWireframe, D3D12_CULL_MODE cullMode) noexcept
{
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	auto& rs = psoDesc.RasterizerState;
	rs = {};
	rs.FillMode = bRenderWireframe ? D3D12_FILL_MODE_WIREFRAME : D3D12_FILL_MODE_SOLID;
	rs.CullMode = cullMode;
	rs.FrontCounterClockwise = FALSE;
	rs.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
	rs.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
	rs.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
	rs.DepthClipEnable = TRUE;
	rs.MultisampleEnable = FALSE;
	rs.AntialiasedLineEnable = FALSE;
	rs.ForcedSampleCount = 0;
	rs.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
}

// Configures blend state for the render target.
void PSO::SetRenderTargetBlendState(D3D12_GRAPHICS_PIPELINE_STATE_DESC& psoDesc, D3D12_RENDER_TARGET_BLEND_DESC blendDesc) noexcept
{
	psoDesc.BlendState = {};
	psoDesc.BlendState.RenderTarget[0] = blendDesc;
}


// Configures depth test state for the pipeline.
void PSO::SetDepthTestState(D3D12_GRAPHICS_PIPELINE_STATE_DESC& psoDesc, DepthTestDesc depthDesc) noexcept
{
	auto& ds = psoDesc.DepthStencilState;
	ds = {};
	ds.DepthEnable = depthDesc.DepthEnable ? TRUE : FALSE;
	ds.DepthWriteMask = depthDesc.DepthWriteMask;
	ds.DepthFunc = depthDesc.DepthFunc;
}


// Configures stencil test state for the pipeline.
void PSO::SetStencilTestState(D3D12_GRAPHICS_PIPELINE_STATE_DESC& psoDesc, StencilTestDesc stencilDesc) noexcept
{
	auto& ds = psoDesc.DepthStencilState;
	ds.StencilEnable = stencilDesc.StencilEnable ? TRUE : FALSE;
	ds.StencilReadMask = stencilDesc.StencilReadMask;
	ds.StencilWriteMask = stencilDesc.StencilWriteMask;

	ds.FrontFace.StencilFunc = stencilDesc.FrontFaceStencilFunc;
	ds.FrontFace.StencilFailOp = stencilDesc.FrontFaceStencilFailOp;
	ds.FrontFace.StencilDepthFailOp = stencilDesc.FrontFaceStencilDepthFailOp;
	ds.FrontFace.StencilPassOp = stencilDesc.FrontFaceStencilPassOp;

	ds.BackFace.StencilFunc = stencilDesc.BackFaceStencilFunc;
	ds.BackFace.StencilFailOp = stencilDesc.BackFaceStencilFailOp;
	ds.BackFace.StencilDepthFailOp = stencilDesc.BackFaceStencilDepthFailOp;
	ds.BackFace.StencilPassOp = stencilDesc.BackFaceStencilPassOp;
}

// Constructs and creates the graphics pipeline state object (PSO)
PSO::PSO(Primitive& vertecies, RootSignature& rootSignature, ShaderCompiler& vertexShader, ShaderCompiler& pixelShader)
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};

	// -- Vertex Data
	std::vector<D3D12_INPUT_ELEMENT_DESC> vertexLayout = vertecies.GetVertexLayout();
	psoDesc.InputLayout.NumElements = static_cast<UINT>(vertexLayout.size());
	psoDesc.InputLayout.pInputElementDescs = vertexLayout.empty() ? nullptr : vertexLayout.data();
	psoDesc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;

	// -- Root Signature
	psoDesc.pRootSignature = rootSignature.GetRaw();

	// -- Shaders
	psoDesc.VS.pShaderBytecode = vertexShader.GetBuffer();
	psoDesc.VS.BytecodeLength = vertexShader.GetSize();
	psoDesc.PS.pShaderBytecode = pixelShader.GetBuffer();
	psoDesc.PS.BytecodeLength = pixelShader.GetSize();

	// -- Rasterizer
	SetRasterizerState(psoDesc, false, D3D12_CULL_MODE_BACK);

	// -- Stream Output (disabled)
	SetStreamOutput(psoDesc);

	// -- Blend State (default single-target config)
	psoDesc.BlendState.AlphaToCoverageEnable = FALSE;
	psoDesc.BlendState.IndependentBlendEnable = FALSE;
	D3D12_RENDER_TARGET_BLEND_DESC rtBlend = {};
	rtBlend.BlendEnable = FALSE;
	rtBlend.BlendOp = D3D12_BLEND_OP_ADD;
	rtBlend.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	rtBlend.SrcBlend = D3D12_BLEND_ONE;
	rtBlend.DestBlend = D3D12_BLEND_ZERO;
	rtBlend.SrcBlendAlpha = D3D12_BLEND_ONE;
	rtBlend.DestBlendAlpha = D3D12_BLEND_ZERO;
	rtBlend.LogicOp = D3D12_LOGIC_OP_NOOP;
	rtBlend.LogicOpEnable = FALSE;
	rtBlend.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	SetRenderTargetBlendState(psoDesc, rtBlend);

	// -- Depth (reversed-Z by default in engine)
	DepthTestDesc depthTestDesc = {};
	depthTestDesc.DepthEnable = true;
	depthTestDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	depthTestDesc.DepthFunc = D3D12_COMPARISON_FUNC_GREATER_EQUAL;
	SetDepthTestState(psoDesc, depthTestDesc);

	// -- Stencil
	StencilTestDesc stencilDesc = {};
	SetStencilTestState(psoDesc, stencilDesc);

	// -- Render target / DSV formats
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = GSwapChain.GetBackBufferFormat();
	psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	// -- Misc
	psoDesc.NodeMask = 0;
	psoDesc.CachedPSO = {};
	psoDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.SampleDesc.Count = 1;
	psoDesc.SampleDesc.Quality = 0;

	// -- Create PSO and report detailed debug information on failure
	HRESULT hr = GRHI.GetDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(m_pso.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
	{
#if defined(_DEBUG)
		// If debug layer is enabled, query InfoQueue for messages (safe retrieval loop)
		ComPtr<ID3D12InfoQueue> infoQueue;
		if (SUCCEEDED(GRHI.GetDevice()->QueryInterface(IID_PPV_ARGS(infoQueue.ReleaseAndGetAddressOf()))))
		{
			const UINT64 numMessages = infoQueue->GetNumStoredMessagesAllowedByRetrievalFilter();
			for (UINT64 i = 0; i < numMessages; ++i)
			{
				SIZE_T messageLength = 0;
				if (FAILED(infoQueue->GetMessage(i, nullptr, &messageLength)) || messageLength == 0)
					continue;

				std::vector<char> messageData(messageLength);
				D3D12_MESSAGE* message = reinterpret_cast<D3D12_MESSAGE*>(messageData.data());
				if (SUCCEEDED(infoQueue->GetMessage(i, message, &messageLength)) && message->pDescription)
				{
					LogMessage(std::string("D3D12 InfoQueue: ") + message->pDescription, ELogType::Info);
				}
			}
			// After logging, clear to avoid duplicate logs on subsequent failures
			infoQueue->ClearStoredMessages();
		}
#endif
		char buf[256];
		std::snprintf(buf, sizeof(buf), "Failed To Create PSO. HRESULT: 0x%08X", static_cast<unsigned int>(hr));
		LogMessage(buf, ELogType::Fatal);
	}

	DebugUtils::SetDebugName(m_pso, L"RHI_PipelineState");
}

PSO::~PSO() noexcept
{
	m_pso.Reset();
}

// Sets the pipeline state object for the current command list.
void PSO::Set() const noexcept
{
	GRHI.GetCommandList()->SetPipelineState(m_pso.Get());
}