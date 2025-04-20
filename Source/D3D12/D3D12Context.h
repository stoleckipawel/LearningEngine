#pragma once
#include "../Vendor/Windows/WinInclude.h"

class D3D12Context
{
public:
	bool Initialize();
	void Shutdown();

	inline ComPointer<ID3D12Device10>& GetDevice() { return m_device; }
	inline ComPointer<ID3D12CommandQueue>& GetQueue() { return m_cmdQueue; }
	inline ComPointer<ID3D12Fence1>& GetFence() { return m_fence; }
private:
	ComPointer<ID3D12Device10> m_device;
	ComPointer<ID3D12CommandQueue> m_cmdQueue;
	ComPointer<ID3D12Fence1> m_fence;
	UINT64 m_fenceValue = 0;

	//Singleton pattern to ensure only one instance of the device exists
public:
	D3D12Context(const D3D12Context&) = delete;
	D3D12Context& operator=(const D3D12Context&) = delete;

	inline static D3D12Context& Get()
	{
		static D3D12Context instance;
		return instance;
	}
private:
	D3D12Context() = default;
};