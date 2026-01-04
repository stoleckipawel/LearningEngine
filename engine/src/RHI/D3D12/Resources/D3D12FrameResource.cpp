#include "PCH.h"
#include "D3D12FrameResource.h"

D3D12FrameResourceManager& D3D12FrameResourceManager::Get() noexcept
{
	static D3D12FrameResourceManager instance;
	return instance;
}
