#pragma once

// Debug utilities: helper to set D3D12 debug names on objects in debug builds.
#include <wrl/client.h>
#include <d3d12.h>

namespace DebugUtils {

    // Set debug name for a raw D3D12 object. No-op in release builds.
    inline void SetDebugName(ID3D12Object* object, const wchar_t* name) noexcept
    {
#if defined(_DEBUG)
        if (object && name)
            object->SetName(name);
#endif
    }

    // Overload for ComPtr
    template<typename T>
    inline void SetDebugName(const Microsoft::WRL::ComPtr<T>& object, const wchar_t* name) noexcept
    {
        SetDebugName(object.Get(), name);
    }

}
