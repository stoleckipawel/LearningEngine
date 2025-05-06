#pragma once
#include "../Vendor/Windows/WinInclude.h"
#include <vector>

struct Vertex
{
	float x;
	float y;
	float u;
	float v;
};

class D3D12Renderer
{
public:
	bool Initialize();
	void Shutdown();
	std::vector<D3D12_INPUT_ELEMENT_DESC> GetVertexLayout();


	//Singleton pattern to ensure only one instance of the debug layer exists
public:
	D3D12Renderer(const D3D12Renderer&) = delete;
	D3D12Renderer& operator=(const D3D12Renderer&) = delete;

	inline static D3D12Renderer& Get()
	{
		static D3D12Renderer instance;
		return instance;
	}
private:
	D3D12Renderer() = default;
};

