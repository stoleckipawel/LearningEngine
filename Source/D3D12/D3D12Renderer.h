#pragma once
#include "../Vendor/Windows/WinInclude.h"
#include "D3D12ImageLoader.h"
#include <vector>
#include "D3D12Shader.h"
#include "D3D12Texture.h"
#include "D3D12Geometry.h"
#include "D3D12PSO.h"

class D3D12Renderer
{
public:
	bool Initialize();
	void Load();
	void Shutdown();
	void Render();

private:
	void Draw(ComPointer<ID3D12GraphicsCommandList7>& cmdList);
	//Texture
	D3D12_RESOURCE_DESC CreateTextureResourceDesc(D3D12ImageLoader::ImageData& textureData);
	void CreateRootSignature();
	void SetViewport(ComPointer<ID3D12GraphicsCommandList7>& cmdList);
	
	//Misc
	void ClearBackBuffer(ComPointer<ID3D12GraphicsCommandList7>& cmdList);
	void SetBackBufferRTV(ComPointer<ID3D12GraphicsCommandList7>& cmdList);
	void SetShaderParams(ComPointer<ID3D12GraphicsCommandList7>& cmdList);

private:
	ComPointer<ID3D12RootSignature> rootSignature = nullptr;
	
	D3D12Texture texture;
	D3D12Geometry vertecies;
	D3D12PSO pso;
};

