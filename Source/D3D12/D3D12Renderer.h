#pragma once
#include "../Vendor/Windows/WinInclude.h"
#include "D3D12ImageLoader.h"
#include <vector>
#include "D3D12Shader.h"
#include "D3D12Texture.h"
#include "D3D12Geometry.h"
#include "D3D12PSO.h"
#include "D3D12DescriptorHeap.h"

class D3D12Renderer
{
public:
	bool Initialize();
	void Load();
	void Shutdown();
	void Render();

private:
	void CreateRootSignature();


	void SetDescriptorHeaps(ComPointer<ID3D12GraphicsCommandList7>& cmdList);
	void SetViewport(ComPointer<ID3D12GraphicsCommandList7>& cmdList);
	void ClearBackBuffer(ComPointer<ID3D12GraphicsCommandList7>& cmdList);
	void SetBackBufferRTV(ComPointer<ID3D12GraphicsCommandList7>& cmdList);
	void SetShaderParams(ComPointer<ID3D12GraphicsCommandList7>& cmdList);

	void Draw(ComPointer<ID3D12GraphicsCommandList7>& cmdList);

private:
	ComPointer<ID3D12RootSignature> rootSignature = nullptr;
	
	D3D12Texture texture;
	D3D12Geometry vertecies;
	D3D12PSO pso;
	D3D12DescriptorHeap srvHeap;
	D3D12DescriptorHeap samplerHeap;
};

