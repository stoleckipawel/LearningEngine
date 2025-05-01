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
		D3D12Window::Get().SetFullScreen(true);

		const char* hello = "Hello World!";

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

		ComPointer<ID3D12Resource2> uploadBuffer, vertexBuffer;
		D3D12Context::D3D12Context::Get().GetDevice()->CreateCommittedResource(&heapDefaultProperties, D3D12_HEAP_FLAG_NONE,
			&resourceDesc, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&uploadBuffer));

		D3D12Context::D3D12Context::Get().GetDevice()->CreateCommittedResource(&heapDefaultProperties, D3D12_HEAP_FLAG_NONE,
			&resourceDesc, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&vertexBuffer));

		//Copy void** -> CPU RESOURCE
		void* uploadBufferAddress;
		D3D12_RANGE uploadRange;
		uploadRange.Begin = 0;
		uploadRange.End = 1023;
		uploadBuffer->Map(0, &uploadRange, &uploadBufferAddress);
		//memcpy(uploadBufferAddress, hello, strlen(hello) + 1);
		uploadBuffer->Unmap(0, nullptr);

		// Copy CPU Resource -> GPU Resource
		auto cmdList = D3D12Context::Get().InitializeCommandList();
		cmdList->CopyBufferRegion(vertexBuffer, 0, uploadBuffer, 0, 1024);
		D3D12Context::Get().ExecuteCommandList();

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

			//#ToDo: Draw
			D3D12Window::Get().EndFrame(cmdList);

			//Finish Drawing & Present
			D3D12Context::Get().ExecuteCommandList();
			D3D12Window::Get().Present();
		}

		
		D3D12Context::Get().Flush(D3D12Window::Get().GetFrameCount());

		D3D12Window::Get().Shutdown();
		D3D12Context::Get().Shutdown();
	}

	D3D12DebugLayer::Get().Shutdown();

    return 0;
}