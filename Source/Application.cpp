#include <iostream>

#include "Vendor/Windows/WinInclude.h"

#include "Debug/D3D12DebugLayer.h"
#include "D3D12/D3D12Context.h"
#include "D3D12/D3D12Window.h"

int main()
{
    D3D12DebugLayer::Get().Initialize();

	if(D3D12Context::Get().Initialize() && D3D12Window::Get().Initialize());
	{
		D3D12Window::Get().SetFullScreen(false);


		// CPU BUFFER 
		D3D12_HEAP_PROPERTIES heapUploadProperties = {};
		heapUploadProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
		heapUploadProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapUploadProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapUploadProperties.CreationNodeMask = 0;
		heapUploadProperties.VisibleNodeMask = 0;

		//GPU Buffer
		D3D12_HEAP_PROPERTIES heapDefaultProperties = {};
		heapDefaultProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
		heapDefaultProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapDefaultProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapDefaultProperties.CreationNodeMask = 0;
		heapDefaultProperties.VisibleNodeMask = 0;

		//Upload & Vertex Buffer Desc
		D3D12_RESOURCE_DESC resourceDesc = {};
		resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resourceDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
		resourceDesc.Width = 1024; 
		resourceDesc.Height = 1;
		resourceDesc.DepthOrArraySize = 1;
		resourceDesc.MipLevels = 1;
		resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
		resourceDesc.SampleDesc.Count = 1;
		resourceDesc.SampleDesc.Quality = 0;
		resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

		ComPointer<ID3D12Resource2> uploadBuffer;
		D3D12Context::D3D12Context::Get().GetDevice()->CreateCommittedResource(&heapUploadProperties, D3D12_HEAP_FLAG_NONE,
			&resourceDesc, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&uploadBuffer));

		ComPointer<ID3D12Resource2> vertexBuffer;
		D3D12Context::D3D12Context::Get().GetDevice()->CreateCommittedResource(&heapDefaultProperties, D3D12_HEAP_FLAG_NONE,
			&resourceDesc, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&vertexBuffer));

		//Vertex Data
		struct Vertex
		{
			float position[3];
		};

		Vertex vertecies[] =
		{
			//T1
			{{-1.0, -1.0, 0.0}},
			{{0.0, 1.0, 0.0}},
			{{1.0, -1.0, 0.0}}
		};

		D3D12_INPUT_ELEMENT_DESC vertexLayout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
		};


		//Copy void** -> CPU RESOURCE
		void* uploadBufferAddress;
		D3D12_RANGE uploadRange;
		uploadRange.Begin = 0;
		uploadRange.End = 1023;
		uploadBuffer->Map(0, &uploadRange, &uploadBufferAddress);
		memcpy(uploadBufferAddress, vertecies, sizeof(Vertex));
		uploadBuffer->Unmap(0, nullptr);

		// Copy CPU Resource -> GPU Resource
		auto cmdList = D3D12Context::Get().InitializeCommandList();
		cmdList->CopyBufferRegion(vertexBuffer, 0, uploadBuffer, 0, 1024);
		D3D12Context::Get().ExecuteCommandList();

		//Vertex
		D3D12_INPUT_LAYOUT_DESC inputLayoutDesc = {};
		inputLayoutDesc.NumElements = _countof(vertexLayout);
		inputLayoutDesc.pInputElementDescs = vertexLayout;

		//Shaders
		Shader rootSignatureShader("RootSignature.cso");
		Shader vertexShader("VertexShader.cso");
		Shader pixelShader("PixelShader.cso");
		
		ComPointer<ID3D12RootSignature> rootSignature;
		D3D12Context::Get().GetDevice()->CreateRootSignature(0, rootSignatureShader.GetBuffer(), rootSignatureShader.GetSize(), IID_PPV_ARGS(&rootSignature));

		//Pipeline state
		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
		psoDesc.pRootSignature = rootSignature;
		psoDesc.InputLayout = inputLayoutDesc;
		psoDesc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
		//VS
		psoDesc.VS.pShaderBytecode = vertexShader.GetBuffer();
		psoDesc.VS.BytecodeLength = vertexShader.GetSize();
		//PS
		psoDesc.PS.pShaderBytecode = pixelShader.GetBuffer();
		psoDesc.PS.BytecodeLength = pixelShader.GetSize();
		//DS
		psoDesc.DS.pShaderBytecode = nullptr;
		psoDesc.DS.BytecodeLength = 0;
		//HS
		psoDesc.HS.pShaderBytecode = nullptr;	
		psoDesc.HS.BytecodeLength = 0;
		//GS
		psoDesc.GS.pShaderBytecode = nullptr;
		psoDesc.GS.BytecodeLength = 0;
		//Rasterizer
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;//WireFrame mode here
		psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;//back face/ twosided
		psoDesc.RasterizerState.FrontCounterClockwise = false;
		psoDesc.RasterizerState.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
		psoDesc.RasterizerState.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
		psoDesc.RasterizerState.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
		psoDesc.RasterizerState.DepthClipEnable = true;
		psoDesc.RasterizerState.MultisampleEnable = false;
		psoDesc.RasterizerState.AntialiasedLineEnable = false;
		psoDesc.RasterizerState.ForcedSampleCount = 0;
		psoDesc.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
		//StreamOutput
		psoDesc.StreamOutput.NumEntries = 0;
		psoDesc.StreamOutput.pSODeclaration = nullptr;
		psoDesc.StreamOutput.NumStrides = 0;
		psoDesc.StreamOutput.pBufferStrides = nullptr;
		psoDesc.StreamOutput.RasterizedStream = 0;
		//NumRenderTargets
		psoDesc.NumRenderTargets = 1;
		psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		psoDesc.DSVFormat = DXGI_FORMAT_UNKNOWN;//DXGI_FORMAT_D24_UNORM_S8_UINT;
		//Blend State
		psoDesc.BlendState.AlphaToCoverageEnable = false;
		psoDesc.BlendState.IndependentBlendEnable = false;//Multiple RenderTarget Varied Blending
		psoDesc.BlendState.RenderTarget[0].BlendEnable = false;
		psoDesc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		psoDesc.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
		psoDesc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
		psoDesc.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
		psoDesc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
		psoDesc.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
		psoDesc.BlendState.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
		psoDesc.BlendState.RenderTarget[0].LogicOpEnable = false;
		psoDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		//Depth Testing
		psoDesc.DepthStencilState.DepthEnable = false;
		psoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
		//Stencil Testing
		psoDesc.DepthStencilState.StencilEnable = false;
		psoDesc.DepthStencilState.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
		psoDesc.DepthStencilState.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;
		psoDesc.DepthStencilState.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
		psoDesc.DepthStencilState.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
		psoDesc.DepthStencilState.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
		psoDesc.DepthStencilState.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		psoDesc.DepthStencilState.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
		psoDesc.DepthStencilState.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
		psoDesc.DepthStencilState.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
		psoDesc.DepthStencilState.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		//NodeMask
		psoDesc.NodeMask = 0;
		//Cached Pso
		psoDesc.CachedPSO.CachedBlobSizeInBytes = 0;
		psoDesc.CachedPSO.pCachedBlob = nullptr;
		//Flags
		psoDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
		//Sample Mask
		psoDesc.SampleMask = UINT_MAX;	
		psoDesc.SampleDesc.Count = 1;
		psoDesc.SampleDesc.Quality = 0;
		ComPointer<ID3D12PipelineState> pso;
		D3D12Context::Get().GetDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pso));

		D3D12_VERTEX_BUFFER_VIEW vertexBufferView = {};
		vertexBufferView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
		vertexBufferView.SizeInBytes = sizeof(vertecies);
		vertexBufferView.StrideInBytes = sizeof(Vertex);

		while (!D3D12Window::Get().GetShouldClose())
		{
			// Process pending window messages
			D3D12Window::Get().Update();

			//Handle Resizing
			if (D3D12Window::Get().GetShouldResize())
			{
				D3D12Context::Get().Flush(D3D12Window::Get().GetFrameCount());
				D3D12Window::Get().Resize();
			}

			//Begin Drawing
			cmdList = D3D12Context::Get().InitializeCommandList();
			 
			//Draw To Window
			D3D12Window::Get().BeginFrame(cmdList);

			//PSO
			cmdList->SetPipelineState(pso);
			cmdList->SetGraphicsRootSignature(rootSignature);

			//Input Assembler
			cmdList->IASetVertexBuffers(0, 1, &vertexBufferView);
			cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			//Rasterizer State: Viewport
			D3D12_VIEWPORT vp;
			vp.TopLeftX = 0;
			vp.TopLeftY = 0;
			vp.Width = D3D12Window::Get().GetWidth();
			vp.Height = D3D12Window::Get().GetHeight();
			vp.MinDepth = 1.0f;
			vp.MaxDepth = 0.0f;
			cmdList->RSSetViewports(1, &vp);
			//Rasterizer State: Scissor  
			D3D12_RECT scissorRect;
			scissorRect.left = 0;
			scissorRect.top = 0;
			scissorRect.right = D3D12Window::Get().GetWidth();
			scissorRect.bottom = D3D12Window::Get().GetHeight();
			cmdList->RSSetScissorRects(1, &scissorRect);

			//ROOT Arguments
			float color[] = { 1.0f, 0.0f, 0.0f };
			cmdList->SetGraphicsRoot32BitConstants(0, 3, color, 0);

			//Draw
			cmdList->DrawInstanced(_countof(vertecies), 1, 0, 0);
			D3D12Window::Get().EndFrame(cmdList);

			//Finish Drawing & Present
			D3D12Context::Get().ExecuteCommandList();
			D3D12Window::Get().Present();
		}

		
		D3D12Context::Get().Flush(D3D12Window::Get().GetFrameCount());

		vertexBuffer->Release();
		uploadBuffer->Release();
		D3D12Window::Get().Shutdown();
		//D3D12Context::Get().Shutdown();
	}

	//D3D12DebugLayer::Get().Shutdown();

    return 0;
}