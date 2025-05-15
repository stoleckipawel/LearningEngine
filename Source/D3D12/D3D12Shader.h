#pragma once

#include <cstring>
#include <cstdlib>
#include <string_view>
#include <filesystem>
#include <fstream>
#include "../Vendor/Windows/WinInclude.h"

class D3D12Shader
{
public:
	D3D12Shader() = default;
	D3D12Shader(std::string_view name);
	~D3D12Shader();

	inline const void* GetBuffer() const { return m_data; }
	inline size_t GetSize() const { return m_size; }
private:
	void* m_data = nullptr;
	size_t m_size = 0;
};

