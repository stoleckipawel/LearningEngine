//------------------------------------------------------------------------------
// Per-Frame CB (b0) — updated once per CPU frame, shared by all draws
//------------------------------------------------------------------------------
cbuffer PerFrameConstantBufferData : register(b0)
{
    uint     FrameIndex;      // Current frame number 
    float    TotalTime;       // Seconds since engine start
    float    DeltaTime;       // Seconds since last frame
    float    _padPerFrame0;           // pad to 16 bytes

    float2 ViewportSize;    // Render target width, height
    float2 ViewportSizeInv; // 1.0 / width, 1.0 / height
    // rest of 256-byte slot is intentionally unused/pad
};

//------------------------------------------------------------------------------
// Per-View CB (b1) — updated per camera/view (main, shadow, reflection, etc.)
//------------------------------------------------------------------------------
cbuffer PerViewConstantBufferData : register(b1)
{
    row_major float4x4 ViewMTX;        // World -> View
    row_major float4x4 ProjectionMTX;  // View -> Clip
    row_major float4x4 ViewProjMTX;    // World -> Clip (precomputed to save GPU work)

    float3   CameraPosition; // World-space camera position
    float    NearZ;          // Near clip plane

    float    FarZ;           // Far clip plane
    float3   CameraDirection;// World-space camera forward
};

//------------------------------------------------------------------------------
// Per-Object VS CB (b2) — updated per draw call (transforms)
//------------------------------------------------------------------------------
cbuffer PerObjectVSConstantBufferData : register(b2)
{
    row_major float4x4 WorldMTX;             // Local -> World
    row_major float4x4 WorldInvTransposeMTX; // For correct normal transformation under non-uniform scale

    // remaining space in the 256-byte slot is reserved for future use
};

//------------------------------------------------------------------------------
// Per-Object PS CB (b3) — updated per draw call (material scalars)
//------------------------------------------------------------------------------
cbuffer PerObjectPSConstantBufferData : register(b3)
{
    float4 BaseColor;  // RGBA base/albedo color or tint

    float    Metallic;   // PBR metallic [0,1]
    float    Roughness;  // PBR roughness [0,1]
    float    F0;         // PBR reflectance at normal incidence
    float    _padPerObjectPS0;   // pad to 16 bytes

    // remaining space reserved
};