#include "D3D12PSO.h"

void D3D12PSO::SetStreamOutput(D3D12_GRAPHICS_PIPELINE_STATE_DESC& psoDesc)
{
	psoDesc.StreamOutput.NumEntries = 0;
	psoDesc.StreamOutput.pSODeclaration = nullptr;
	psoDesc.StreamOutput.NumStrides = 0;
	psoDesc.StreamOutput.pBufferStrides = nullptr;
	psoDesc.StreamOutput.RasterizedStream = 0;
}

void D3D12PSO::SetRasterizerState(D3D12_GRAPHICS_PIPELINE_STATE_DESC& psoDesc, bool bRenderWireframe, D3D12_CULL_MODE cullMode)
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

void D3D12PSO::SetRenderTargetBlendState(D3D12_GRAPHICS_PIPELINE_STATE_DESC& psoDesc, D3D12_RENDER_TARGET_BLEND_DESC blendDesc)
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

void D3D12PSO::SetDepthTestState(D3D12_GRAPHICS_PIPELINE_STATE_DESC& psoDesc, D3D12_DEPTH_TEST_DESC depthDesc)
{
	psoDesc.DepthStencilState.DepthEnable = depthDesc.DepthEnable;
	psoDesc.DepthStencilState.DepthWriteMask = depthDesc.DepthWriteMask;
	psoDesc.DepthStencilState.DepthFunc = depthDesc.DepthFunc;
}

void D3D12PSO::SetStencilTestState(D3D12_GRAPHICS_PIPELINE_STATE_DESC& psoDesc, D3D12_STENCIL_TEST_DESC stencilDesc)
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

void D3D12PSO::Create(D3D12Geometry& vertecies, 
					ID3D12RootSignature* rootSignature,
					D3D12ShaderCompiler& vertexShader,
					D3D12ShaderCompiler& pixelShader)
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};

	// -- Vertex Data
	std::vector<D3D12_INPUT_ELEMENT_DESC> vertexLayout = vertecies.GetVertexLayout();
	psoDesc.InputLayout.NumElements = vertexLayout.size();
	psoDesc.InputLayout.pInputElementDescs = vertexLayout.data();
	psoDesc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;

	// -- Root Signature
	psoDesc.pRootSignature = rootSignature;

	//VS	
	psoDesc.VS.pShaderBytecode = vertexShader.GetBuffer();
	psoDesc.VS.BytecodeLength = vertexShader.GetSize();
	//PS
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
	if (FAILED(D3D12Context::Get().GetDevice()->CreateGraphicsPipelineState(
		&psoDesc,
		IID_PPV_ARGS(&pso))))
	{
		std::string message = "PSO Create: Failed To Create PSO";
		LogError(message);
	}
}

void D3D12PSO::Set(ComPointer<ID3D12GraphicsCommandList7>& cmdList)
{
	//PSO
	cmdList->SetPipelineState(pso);
}