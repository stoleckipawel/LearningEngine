#include "D3D12Context.h"

bool D3D12Context::Initialize()
{
	if (FAILED(D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&m_device))))
	{
		return false;
	}

	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc = {};
	cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_HIGH;
	cmdQueueDesc.NodeMask = 0;
	cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

	if (FAILED(m_device->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&m_cmdQueue))))
	{
		return false;
	}
			
	if (FAILED(m_device->CreateFence(m_fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence))))
	{
		return false;
	}

	if (FAILED(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_cmdAllocator))))
	{
		return false;
	}

	if (FAILED(m_device->CreateCommandList1(0, D3D12_COMMAND_LIST_TYPE_DIRECT, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(&m_cmdList))))
	{
		return false;
	}


    return true;
}

void D3D12Context::Shutdown()
{
	m_cmdAllocator.Release();
	m_cmdList.Release();

	if (m_fenceEvent)
	{
		CloseHandle(m_fenceEvent);
	}

	m_cmdQueue.Release();
	m_device.Release();
}

void D3D12Context::SignalAndWait()
{
   m_cmdQueue->Signal(m_fence, ++m_fenceValue);

   // Wait until the fence is completed (CPU & GPU synchronization)
   if (m_fence->GetCompletedValue() < m_fenceValue)
   {
       m_fenceEvent = CreateEvent(nullptr, false, false, nullptr);
       if (SUCCEEDED(m_fence->SetEventOnCompletion(m_fenceValue, m_fenceEvent)))
       {
		   if (WaitForSingleObject(m_fenceEvent, 20000) != WAIT_OBJECT_0)
		   {
			   OutputDebugStringW(L"Timeout waiting for fence completion.\n");
			   std::exit(-1);
		   }

		   CloseHandle(m_fenceEvent);
       }
       else
       {
           OutputDebugStringW(L"Failed to set event on fence completion.\n");
           std::exit(-1); 
       }
   }
}

ComPointer<ID3D12GraphicsCommandList7>& D3D12Context::InitializeCommandList()
{
	m_cmdAllocator->Reset();
	m_cmdList->Reset(m_cmdAllocator, nullptr);
	return m_cmdList;
}

void D3D12Context::ExecuteCommandList()
{
	if (SUCCEEDED(m_cmdList->Close()))
	{
		ID3D12CommandList* commandLists[] = { m_cmdList };
		m_cmdQueue->ExecuteCommandLists(1, commandLists);
		SignalAndWait();
	};
}
