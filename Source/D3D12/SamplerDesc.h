#pragma once

#include "../Vendor/Windows/WinInclude.h"
#include "RHI.h"

class FSamplerDesc : public D3D12_STATIC_SAMPLER_DESC
{
public:
    FSamplerDesc();
    D3D12_CPU_DESCRIPTOR_HANDLE CreateDescriptor(void);
    D3D12_CPU_DESCRIPTOR_HANDLE Handle;  
    UINT HandleIndex = 0;
};

