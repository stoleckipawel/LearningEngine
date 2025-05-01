#pragma once

#include <cstring>
#include <cstdlib>
#include <string_view>
#include <filesystem>
#include <fstream>
#include "../Vendor/Windows/WinInclude.h"

class Shader
{
public:
	Shader(std::string_view name);
	~Shader();

	inline const void* GetBuffer() const { return m_data; }
	inline size_t GetSize() const { return m_size; }
private:
	void* m_data = nullptr;
	size_t m_size = 0;
};

