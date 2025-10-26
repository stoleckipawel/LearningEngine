#include "Texture.h"


void Texture::Load(const std::filesystem::path& imagePath, ID3D12GraphicsCommandList* cmdList, ID3D12DescriptorHeap* srvHeap, UINT DescriptorHandleIndex)
{
	//ToDo:Switch to DirectXTex for better format support and mip generation
	//ImageLoader::LoadImageFromDisk("Assets/Textures/ColorCheckerBoard.png", textureData);

    //CreateResource();
    //UploadToGPU(cmdList);
    //CreateSRV(srvHeap, DescriptorHandleIndex);
}

void Texture::CreateResource()
{
	//Texture Resource Desc
	D3D12_RESOURCE_DESC texResourceDesc = {};
	{
		texResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		texResourceDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
		texResourceDesc.Width = textureData.width;
		texResourceDesc.Height = textureData.height;
		texResourceDesc.DepthOrArraySize = 1;
		texResourceDesc.MipLevels = 1;//ToDo: Generate Mips
		texResourceDesc.Format = textureData.dxgiPixelFormat;
		texResourceDesc.SampleDesc.Count = 1;
		texResourceDesc.SampleDesc.Quality = 0;
		texResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		texResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	}

		D3D12_HEAP_PROPERTIES heapDefaultProperties = {};
		heapDefaultProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
		heapDefaultProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapDefaultProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapDefaultProperties.CreationNodeMask = 0;
		heapDefaultProperties.VisibleNodeMask = 0;

	ThrowIfFailed(GRHI.Device->CreateCommittedResource(
		&heapDefaultProperties,
		D3D12_HEAP_FLAG_NONE,
		&texResourceDesc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(&textureResource)), 
		"Texture: Failed To Create Texture Resource");

	UINT64 uploadBufferSize = GetRequiredIntermediateSize(textureResource.Get(), 0, 1);//update when adding mips

	D3D12_HEAP_PROPERTIES heapUploadProperties = {};
	heapDefaultProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
	heapDefaultProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapDefaultProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapDefaultProperties.CreationNodeMask = 0;
	heapDefaultProperties.VisibleNodeMask = 0;

	CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);

	ThrowIfFailed(GRHI.Device->CreateCommittedResource(
		&heapUploadProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&uploadResource)),
		"Texture: Failed To Create Upload Buffer");	
}

void Texture::UploadToGPU(ID3D12GraphicsCommandList* cmdList)
{
    D3D12_SUBRESOURCE_DATA sub = {};
	{
		sub.pData = textureData.data.data();
		sub.RowPitch = textureData.width * ((textureData.bitsPerPixel + 7) / 8);
		sub.SlicePitch = sub.RowPitch * textureData.height;	
	}

	UpdateSubresources(cmdList, textureResource.Get(), uploadResource.Get(), 0, 0, 1, &sub);

    // Barrier to PIXEL_SHADER_RESOURCE
    CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        textureResource.Get(),
        D3D12_RESOURCE_STATE_COPY_DEST,
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

    cmdList->ResourceBarrier(1, &barrier);	
}

void Texture::CreateSRV(ID3D12DescriptorHeap* srvHeap, UINT DescriptorHandleIndex)
{
    UINT descriptorSize = GRHI.Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    D3D12_CPU_DESCRIPTOR_HANDLE cpuStart = srvHeap->GetCPUDescriptorHandleForHeapStart();
    cpuStart.ptr += DescriptorHandleIndex * descriptorSize;

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = textureData.dxgiPixelFormat;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;//ToDo: Update when adding mips
	srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
	srvDesc.Texture2D.PlaneSlice = 0;

	GRHI.Device->CreateShaderResourceView(
		textureResource,
		&srvDesc,
		srvHeap->GetCPUDescriptorHandleForHeapStart());//Popraw!!!!!!!!!!!!!!

    DescriptorHandleIndex = DescriptorHandleIndex;		
}

void Texture::Release()
{
    //textureResource.Release();
    //uploadResource.Release();
}