// Windows macros
#define NOMINMAX
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers
#endif

// Standard library
#include <chrono>
#include <cstdlib>
#include <vector>
#include <filesystem>
#include <string>
#include <cstring>
#include <string_view>
#include <fstream>
#include <iostream>
#include <memory>
#include <algorithm>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <cmath>
#include <functional>
#include <cstdint>

// Windows
#include <Windows.h>
#include "wincodec.h"
#include <winnt.h>
#include <wingdi.h>

// DirectX
#include <d3d12.h>
#include <dxcapi.h>
#include <DirectXMath.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <d3dx12.h>
#include <wrl/client.h>

// DirectX debug
#ifdef _DEBUG
#include <d3d12sdklayers.h>
#include <dxgidebug.h>
#endif

using namespace DirectX;
using namespace Microsoft::WRL;

#include "Error.h"
#include "EngineConfig.h"
