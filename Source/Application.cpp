#include <iostream>

#include "Vendor/Windows/WinInclude.h"
#include "Debug/D3D12DebugLayer.h"
#include "D3D12/D3D12Context.h"
#include "D3D12/D3D12Window.h"
#include "D3D12/D3D12ImageLoader.h"

int main()
{
    D3D12DebugLayer::Get().Initialize();

	if(D3D12Context::Get().Initialize() && D3D12Window::Get().Initialize())
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

		//Vertex Data
		struct Vertex
		{
			float x;
			float y;
			float u;
			float v;
		};

		Vertex vertecies[] =
		{
			//T1
			{-1.0, -1.0, 0.0, 1.0},
			{0.0, 1.0, 0.5, 0.0},
			{1.0, -1.0, 1.0, 1.0}
		};

		D3D12_INPUT_ELEMENT_DESC vertexLayout[] =
		{
			{ "Position", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TexCoord", 0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(float) * 2, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
		};

		//Texture
		D3D12ImageLoader::ImageData textureData;
		D3D12ImageLoader::LoadImageFromDisk("Assets/Textures/ColorCheckerBoard.png", textureData);

		uint32_t textureStride = textureData.width * ((textureData.bitsPerPixel + 7) / 8);
		uint32_t textureSize = textureStride * textureData.height;

		//Upload & Vertex Buffer Desc
		D3D12_RESOURCE_DESC rdv = {};
		rdv.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		rdv.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
		rdv.Width = 1024;
		rdv.Height = 1;
		rdv.DepthOrArraySize = 1;
		rdv.MipLevels = 1;
		rdv.Format = DXGI_FORMAT_UNKNOWN;
		rdv.SampleDesc.Count = 1;
		rdv.SampleDesc.Quality = 0;
		rdv.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		rdv.Flags = D3D12_RESOURCE_FLAG_NONE;

		D3D12_RESOURCE_DESC rdu{};
		rdu.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		rdu.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
		rdu.Width = textureSize + 1024;
		rdu.Height = 1;
		rdu.DepthOrArraySize = 1;
		rdu.MipLevels = 1;
		rdu.Format = DXGI_FORMAT_UNKNOWN;
		rdu.SampleDesc.Count = 1;
		rdu.SampleDesc.Quality = 0;
		rdu.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		rdu.Flags = D3D12_RESOURCE_FLAG_NONE;

		ComPointer<ID3D12Resource2> uploadBuffer;
		D3D12Context::Get().GetDevice()->CreateCommittedResource(&heapUploadProperties, D3D12_HEAP_FLAG_NONE,
			&rdu, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&uploadBuffer));

		ComPointer<ID3D12Resource2> vertexBuffer;
		D3D12Context::Get().GetDevice()->CreateCommittedResource(&heapDefaultProperties, D3D12_HEAP_FLAG_NONE,
			&rdv, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&vertexBuffer));


		// === Texture ===
		D3D12_RESOURCE_DESC rdt{};
		rdt.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		rdt.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
		rdt.Width = textureData.width;
		rdt.Height = textureData.height;
		rdt.DepthOrArraySize = 1;
		rdt.MipLevels = 1;
		rdt.Format = textureData.dxgiPixelFormat;
		rdt.SampleDesc.Count = 1;
		rdt.SampleDesc.Quality = 0;
		rdt.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		rdt.Flags = D3D12_RESOURCE_FLAG_NONE;

		ComPointer<ID3D12Resource2> textureBuffer;
		D3D12Context::Get().GetDevice()->CreateCommittedResource(&heapDefaultProperties, D3D12_HEAP_FLAG_NONE,
			&rdt, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&textureBuffer));

		//Descriptor Heap for Textures
		D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
		descHeapDesc.NumDescriptors = 8;
		descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		descHeapDesc.NodeMask = 0;

		ComPointer<ID3D12DescriptorHeap> descHeap;
		D3D12Context::Get().GetDevice()->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&descHeap));

		//SRV
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = textureData.dxgiPixelFormat;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;
		srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
		srvDesc.Texture2D.PlaneSlice = 0;
		D3D12Context::Get().GetDevice()->CreateShaderResourceView(textureBuffer, &srvDesc, descHeap->GetCPUDescriptorHandleForHeapStart());
		

		//Copy void** -> CPU RESOURCE
		char* uploadBufferAddress;
		D3D12_RANGE uploadRange;
		uploadRange.Begin = 0;
		uploadRange.End = 1024 + textureSize;
		uploadBuffer->Map(0, &uploadRange, (void**)&uploadBufferAddress);
		memcpy(&uploadBufferAddress[0], textureData.data.data(), textureSize);
		memcpy(&uploadBufferAddress[textureSize], vertecies, sizeof(vertecies));
		uploadBuffer->Unmap(0, &uploadRange);

		// Copy CPU Resource -> GPU Resource
		auto cmdList = D3D12Context::Get().InitializeCommandList();
		cmdList->CopyBufferRegion(vertexBuffer, 0, uploadBuffer, textureSize, 1024);

		D3D12_BOX textureSizeAsBox;
		textureSizeAsBox.left = textureSizeAsBox.top = textureSizeAsBox.front = 0;
		textureSizeAsBox.right = textureData.width;
		textureSizeAsBox.bottom = textureData.height;
		textureSizeAsBox.back = 1;

		D3D12_TEXTURE_COPY_LOCATION txtcSrc;
		
		txtcSrc.pResource = uploadBuffer;
		txtcSrc.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
		txtcSrc.PlacedFootprint.Offset = 0;
		txtcSrc.PlacedFootprint.Footprint.Width = textureData.width;
		txtcSrc.PlacedFootprint.Footprint.Height = textureData.height;
		txtcSrc.PlacedFootprint.Footprint.Depth = 1;
		txtcSrc.PlacedFootprint.Footprint.RowPitch = textureStride;
		txtcSrc.PlacedFootprint.Footprint.Format = textureData.dxgiPixelFormat;
		
		D3D12_TEXTURE_COPY_LOCATION	txtcDst;
		txtcDst.pResource = textureBuffer;
		txtcDst.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
		txtcDst.SubresourceIndex = 0;
		cmdList->CopyTextureRegion(&txtcDst, 0, 0, 0, &txtcSrc, &textureSizeAsBox);

		D3D12Context::Get().ExecuteCommandList();

		//Shaders
		Shader rootSignatureShader("RootSignature.cso");
		Shader vertexShader("VertexShader.cso");
		Shader pixelShader("PixelShader.cso");

		//Create Root Signature
		ComPointer<ID3D12RootSignature> rootSignature;
		D3D12Context::Get().GetDevice()->CreateRootSignature(0, rootSignatureShader.GetBuffer(), rootSignatureShader.GetSize(), IID_PPV_ARGS(&rootSignature));

		//Pipeline state
		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
		psoDesc.pRootSignature = rootSignature;
		psoDesc.InputLayout.NumElements = _countof(vertexLayout);
		psoDesc.InputLayout.pInputElementDescs = vertexLayout;
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
		psoDesc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
		psoDesc.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
		psoDesc.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
		psoDesc.BlendState.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
		psoDesc.BlendState.RenderTarget[0].LogicOpEnable = false;
		psoDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		//Depth Testing
		psoDesc.DepthStencilState.DepthEnable = false;
		psoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
		psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		//Stencil Testing
		psoDesc.DepthStencilState.StencilEnable = false;
		psoDesc.DepthStencilState.StencilReadMask = 0;
		psoDesc.DepthStencilState.StencilWriteMask = 0;
		psoDesc.DepthStencilState.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		psoDesc.DepthStencilState.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
		psoDesc.DepthStencilState.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
		psoDesc.DepthStencilState.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
		psoDesc.DepthStencilState.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
		psoDesc.DepthStencilState.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
		psoDesc.DepthStencilState.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
		//NodeMask
		psoDesc.NodeMask = 0;
		//Cached Pso
		psoDesc.CachedPSO.CachedBlobSizeInBytes = 0;
		psoDesc.CachedPSO.pCachedBlob = nullptr;
		//Flags
		psoDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
		//Sample Mask
		psoDesc.SampleMask = 0xFFFFFFFF;
		psoDesc.SampleDesc.Count = 1;
		psoDesc.SampleDesc.Quality = 0;
		ComPointer<ID3D12PipelineState> pso;
		D3D12Context::Get().GetDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pso));

		D3D12_VERTEX_BUFFER_VIEW vertexBufferView = {};
		vertexBufferView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
		vertexBufferView.SizeInBytes = sizeof(Vertex) * _countof(vertecies);
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

			//Update
			float color[] = { 1.0f, 1.0f, 0.0f };

			static float angle = 0.0f;
			angle += 0.1;
			struct Correction
			{
				float AspectRatio;
				float Zoom;
				float sinAngle;
				float cosAngle;
			};
			Correction correctionData;
			correctionData.AspectRatio = (float)D3D12Window::Get().GetWidth() / (float)D3D12Window::Get().GetHeight();
			correctionData.Zoom = 0.8f;
			correctionData.sinAngle = sinf(angle);
			correctionData.sinAngle = cosf(angle);


			//ROOT 
			cmdList->SetGraphicsRoot32BitConstants(0, 3, color, 0);
			cmdList->SetGraphicsRoot32BitConstants(1, 4, &correctionData, 0);
			cmdList->SetGraphicsRootDescriptorTable(2, descHeap->GetGPUDescriptorHandleForHeapStart());
			cmdList->SetDescriptorHeaps(1, &descHeap);

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
		//pso->Release();
		//D3D12Window::Get().Shutdown();
		//D3D12Context::Get().Shutdown();
	}

	//D3D12DebugLayer::Get().Shutdown();
}