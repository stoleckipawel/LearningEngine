#pragma once
#include <d3d12.h>
#include <wrl/client.h>
#include <cstdint>
#include <array>
#include "ConstantBufferData.h"
#include "ConstantBuffer.h"
#include "Primitive.h"


class ConstantBufferManager
{
public:
    void Initialize();

    // Accessors
    const ConstantBuffer<PerFrameConstantBufferData>* GetPerFrameConstantBuffer() const noexcept { return PerFrameConstantBuffer[GSwapChain.GetFrameInFlightIndex()].get(); }
    const ConstantBuffer<PerViewConstantBufferData>* GetPerViewConstantBuffer() const noexcept { return PerViewConstantBuffer[GSwapChain.GetFrameInFlightIndex()].get(); }
    const ConstantBuffer<PerObjectVSConstantBufferData>* GetPerObjectVSConstantBuffer() const noexcept { return PerObjectVSConstantBuffer[GSwapChain.GetFrameInFlightIndex()].get(); }
    const ConstantBuffer<PerObjectPSConstantBufferData>* GetPerObjectPSConstantBuffer() const noexcept { return PerObjectPSConstantBuffer[GSwapChain.GetFrameInFlightIndex()].get(); }

    // -------------------------------------------------------------------------
    // Update methods: copy POD data to GPU memory, return GPU VA for binding
    // -------------------------------------------------------------------------

    // PerFrame: updated once per frame by Renderer using Timer + Viewport data
    void UpdatePerFrame();

    // PerView: updated per camera/view by Renderer using Camera data
    void UpdatePerView();

    // PerObjectVS: updated per draw by Renderer using Primitive transform data
    void UpdatePerObjectVS(const Primitive& primitive);

    // PerObjectPS: updated per draw by Renderer using Material data
    void UpdatePerObjectPS();

private:
    // Per Frame constant buffers
    std::unique_ptr<ConstantBuffer<PerFrameConstantBufferData>> PerFrameConstantBuffer[EngineSettings::FramesInFlight];

    // Per View constant buffers
    std::unique_ptr<ConstantBuffer<PerViewConstantBufferData>> PerViewConstantBuffer[EngineSettings::FramesInFlight];

    //Per Object constant buffers
    std::unique_ptr<ConstantBuffer<PerObjectVSConstantBufferData>> PerObjectVSConstantBuffer[EngineSettings::FramesInFlight];
    std::unique_ptr<ConstantBuffer<PerObjectPSConstantBufferData>> PerObjectPSConstantBuffer[EngineSettings::FramesInFlight];    
};

extern ConstantBufferManager GConstantBufferManager;