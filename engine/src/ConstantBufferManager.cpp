#include "PCH.h"
#include "ConstantBufferManager.h"
#include "Camera.h"
#include "Timer.h"
#include "Window.h"
#include "SwapChain.h"

ConstantBufferManager GConstantBufferManager;

void ConstantBufferManager::Initialize()
{
    // Create constant buffers
    for (unsigned i = 0; i < EngineSettings::FramesInFlight; ++i)
    {
        PerFrameConstantBuffer[i] = std::make_unique<ConstantBuffer<PerFrameConstantBufferData>>();
        PerViewConstantBuffer[i] = std::make_unique<ConstantBuffer<PerViewConstantBufferData>>();
        PerObjectVSConstantBuffer[i] = std::make_unique<ConstantBuffer<PerObjectVSConstantBufferData>>();
        PerObjectPSConstantBuffer[i] = std::make_unique<ConstantBuffer<PerObjectPSConstantBufferData>>();
    }    
}

// PerFrame: updated once per frame by Renderer using Timer + Viewport data
void ConstantBufferManager::UpdatePerFrame()
{
    PerFrameConstantBufferData perFrameCBData;
    perFrameCBData.FrameIndex = GTimer.GetFrameCount();
    perFrameCBData.TotalTime = GTimer.GetTotalTime();
    perFrameCBData.DeltaTime = GTimer.GetDelta();
    perFrameCBData._padPerFrame0 = 0.0f; 

    perFrameCBData.ViewportSize = GWindow.GetViewportSize();
    perFrameCBData.ViewportSizeInv = GWindow.GetViewportSizeInv();
    PerFrameConstantBuffer[GSwapChain.GetFrameInFlightIndex()]->Update(perFrameCBData);
}

// PerView: updated per camera/view by Renderer using Camera data
void ConstantBufferManager::UpdatePerView()
{
    PerViewConstantBufferData perViewCBData;
    perViewCBData.CameraPosition = GCamera.GetPosition();
    perViewCBData.CameraDirection = GCamera.GetDirection();
    perViewCBData.NearZ = GCamera.GetNearZ();
    perViewCBData.FarZ = GCamera.GetFarZ();

    const XMMATRIX view = GCamera.GetViewMatrix();
    const XMMATRIX proj = GCamera.GetProjectionMatrix();
    const XMMATRIX viewProj = XMMatrixMultiply(view, proj);

    XMStoreFloat4x4(&perViewCBData.ViewMTX, view);
    XMStoreFloat4x4(&perViewCBData.ProjectionMTX, proj);
    XMStoreFloat4x4(&perViewCBData.ViewProjMTX, viewProj);

	PerViewConstantBuffer[GSwapChain.GetFrameInFlightIndex()]->Update(perViewCBData);    
}

// PerObjectVS: updated per draw by Renderer using Primitive transform data
void ConstantBufferManager::UpdatePerObjectVS(const Primitive& primitive)
{
    PerObjectVSConstantBufferData perObjectVSData;
    
    // World matrix: local -> world transform
    const XMMATRIX world = primitive.GetWorldMatrix();
    XMStoreFloat4x4(&perObjectVSData.WorldMTX, world);
    
    // Inverse-transpose: for correct normal transformation under non-uniform scale
    // Normal = normalize(mul(WorldInvTranspose, normal))
    const XMMATRIX worldInvTranspose = primitive.GetWorldInverseTransposeMatrix();
    XMStoreFloat4x4(&perObjectVSData.WorldInvTransposeMTX, worldInvTranspose);
    
    PerObjectVSConstantBuffer[GSwapChain.GetFrameInFlightIndex()]->Update(perObjectVSData);
}

// PerObjectPS: updated per draw by Renderer using Material data
void ConstantBufferManager::UpdatePerObjectPS()
{
	PerObjectPSConstantBufferData perObjectPSData;
    perObjectPSData.BaseColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f); 
    perObjectPSData.Metallic = 0.0f;  
    perObjectPSData.Roughness = 0.0f;  
    perObjectPSData.F0 = 0.0f;         
    perObjectPSData._padPerObjectPS0 = 0.0f; 
	PerObjectPSConstantBuffer[GSwapChain.GetFrameInFlightIndex()]->Update(perObjectPSData);
}

