#include <iostream>
#include "Vendor/Windows/WinInclude.h"
#include "D3D12/D3D12DebugLayer.h"
#include "D3D12/D3D12Context.h"
#include "D3D12/D3D12Window.h"
#include "D3D12/D3D12ImageLoader.h"
#include "D3D12/D3D12Renderer.h"

int main()
{
	if(D3D12Renderer::Get().Initialize())
	{
		/*
		//Texture
		D3D12ImageLoader::ImageData textureData;
		D3D12ImageLoader::LoadImageFromDisk("Assets/Textures/ColorCheckerBoard.png", textureData);

		uint32_t textureStride = textureData.width * ((textureData.bitsPerPixel + 7) / 8);
		uint32_t textureSize = textureStride * textureData.height;
		// === Texture ===
		D3D12_RESOURCE_DESC textureResourceDesc = D3D12Renderer::Get().CreateTextureResourceDesc(textureData);

		ComPointer<ID3D12Resource2> textureResource;
		D3D12Context::Get().GetDevice()->CreateCommittedResource(
			&heapDefaultProperties,
			D3D12_HEAP_FLAG_NONE,
			&textureResourceDesc,
			D3D12_RESOURCE_STATE_COMMON,
			nullptr,
			IID_PPV_ARGS(&textureResource));

		//SRV
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = textureData.dxgiPixelFormat;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;
		srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
		srvDesc.Texture2D.PlaneSlice = 0;
		D3D12Context::Get().GetDevice()->CreateShaderResourceView(
			textureResource,
			&srvDesc,
			descHeap->GetCPUDescriptorHandleForHeapStart());
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
		txtcDst.pResource = textureResource;
		txtcDst.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
		txtcDst.SubresourceIndex = 0;
		cmdList->CopyTextureRegion(&txtcDst, 0, 0, 0, &txtcSrc, &textureSizeAsBox);

		//Descriptor Heap for Textures
		D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
		descHeapDesc.NumDescriptors = 8;
		descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		descHeapDesc.NodeMask = 0;

		ComPointer<ID3D12DescriptorHeap> descHeap;
		D3D12Context::Get().GetDevice()->CreateDescriptorHeap(
			&descHeapDesc,
			IID_PPV_ARGS(&descHeap));
		*/

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

		D3D12_VERTEX_BUFFER_VIEW vertexBufferView = D3D12Renderer::Get().LoadVertecies(heapDefaultProperties, heapUploadProperties);

		//Shaders
		Shader rootSignatureShader("RootSignature.cso");
		Shader vertexShader("VertexShader.cso");
		Shader pixelShader("PixelShader.cso");

		//Create Root Signature
		ComPointer<ID3D12RootSignature> rootSignature;
		D3D12Context::Get().GetDevice()->CreateRootSignature(
			0, 
			rootSignatureShader.GetBuffer(), 
			rootSignatureShader.GetSize(), 
			IID_PPV_ARGS(&rootSignature));

		//Pipeline state
		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
		psoDesc.pRootSignature = rootSignature;
		//Vertex Layout
		std::vector<D3D12_INPUT_ELEMENT_DESC> vertexLayout = D3D12Renderer::Get().GetVertexLayout();
		psoDesc.InputLayout.NumElements = vertexLayout.size();
		psoDesc.InputLayout.pInputElementDescs = vertexLayout.data();
		psoDesc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
		//VS
		psoDesc.VS.pShaderBytecode = vertexShader.GetBuffer();
		psoDesc.VS.BytecodeLength = vertexShader.GetSize();
		//PS
		psoDesc.PS.pShaderBytecode = pixelShader.GetBuffer();
		psoDesc.PS.BytecodeLength = pixelShader.GetSize();
		//Rasterizer
		D3D12Renderer::Get().SetRasterizerState(psoDesc, false, D3D12_CULL_MODE_NONE);
		//StreamOutput
		D3D12Renderer::Get().SetStreamOutput(psoDesc);
		//NumRenderTargets
		psoDesc.NumRenderTargets = 1;
		psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		psoDesc.DSVFormat = DXGI_FORMAT_UNKNOWN;//DXGI_FORMAT_D24_UNORM_S8_UINT;
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
		D3D12Renderer::Get().SetRenderTargetBlendState(psoDesc, renderTargetBlendDesc);
		
		D3D12_DEPTH_TEST_DESC depthTestDesc;
		{
			depthTestDesc.DepthEnable = false;
			depthTestDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
			depthTestDesc.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		}
		D3D12Renderer::Get().SetDepthTestState(psoDesc, depthTestDesc);

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
		D3D12Renderer::Get().SetStencilTestState(psoDesc, stencilTestDesc);
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
		//Create PSO
		ComPointer<ID3D12PipelineState> pso;
		D3D12Context::Get().GetDevice()->CreateGraphicsPipelineState(
			&psoDesc, 
			IID_PPV_ARGS(&pso));

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
			auto cmdList = D3D12Context::Get().InitializeCommandList();
			 
			//Draw To Window
			D3D12Window::Get().BeginFrame(cmdList);

			//PSO
			cmdList->SetPipelineState(pso);
			cmdList->SetGraphicsRootSignature(rootSignature);
			//Input Assembler
			cmdList->IASetVertexBuffers(0, 1, &vertexBufferView);
			cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			//Rasterizer State: Viewport
			D3D12_VIEWPORT viewport = D3D12Window::Get().GetDefaultViewport();
			cmdList->RSSetViewports(1, &viewport);
			//Rasterizer State: Scissor  
			D3D12_RECT scissorRect = D3D12Window::Get().GetDefaultScissorRect();
			cmdList->RSSetScissorRects(1, &scissorRect);


			//ROOT 
			float color[] = { 1.0f, 1.0f, 0.0f };
			cmdList->SetGraphicsRoot32BitConstants(0, 3, color, 0);
			//cmdList->SetGraphicsRootDescriptorTable(1, descHeap->GetGPUDescriptorHandleForHeapStart());
			//cmdList->SetDescriptorHeaps(1, &descHeap);

			//Draw
			cmdList->DrawInstanced(vertexBufferView.SizeInBytes / vertexBufferView.StrideInBytes, 1, 0, 0);
			D3D12Window::Get().EndFrame(cmdList);

			//Finish Drawing & Present
			D3D12Context::Get().ExecuteCommandList();
			D3D12Window::Get().Present();
		}

		
		D3D12Context::Get().Flush(D3D12Window::Get().GetFrameCount());

		//vertexBuffer->Release();
		//uploadBuffer->Release();
		//pso->Release();
		//D3D12Window::Get().Shutdown();
		//D3D12Context::Get().Shutdown();
	}

	//D3D12DebugLayer::Get().Shutdown();
}