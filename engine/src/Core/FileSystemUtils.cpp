#include "PCH.h"

#include "Core/FileSystemUtils.h"

#include <cstdlib>
#include <string>

#if defined(_WIN32)
	#define WIN32_LEAN_AND_MEAN
	#define NOMINMAX
	#include <Windows.h>
#endif

namespace Engine::FileSystem
{

	std::filesystem::path NormalizePath(const std::filesystem::path& path)
	{
		if (path.empty())
		{
			return {};
		}

		auto normalized = path.is_relative() ? std::filesystem::absolute(path) : path;
		normalized.make_preferred();

		std::error_code ec;
		if (auto canonical = std::filesystem::weakly_canonical(normalized, ec); !ec)
		{
			return canonical;
		}
		return normalized;
	}

	std::filesystem::path GetExecutableDirectory()
	{
#if defined(_WIN32)
		wchar_t buffer[MAX_PATH];
		const DWORD len = GetModuleFileNameW(nullptr, buffer, MAX_PATH);
		if (len > 0 && len < MAX_PATH)
		{
			return std::filesystem::path(buffer).parent_path();
		}
#endif
		return std::filesystem::current_path();
	}

	std::optional<std::filesystem::path> TryGetEnvironmentPath(std::string_view variableName)
	{
		if (variableName.empty())
		{
			return std::nullopt;
		}

#if defined(_WIN32)
		std::wstring wideName(variableName.begin(), variableName.end());

		DWORD requiredChars = GetEnvironmentVariableW(wideName.c_str(), nullptr, 0);
		if (requiredChars == 0)
		{
			return std::nullopt;
		}

		std::wstring value;
		value.resize(static_cast<size_t>(requiredChars));

		requiredChars = GetEnvironmentVariableW(wideName.c_str(), value.data(), requiredChars);
		if (requiredChars == 0)
		{
			return std::nullopt;
		}

		value.resize(static_cast<size_t>(requiredChars));
		if (value.empty())
		{
			return std::nullopt;
		}

		return std::filesystem::path(value);
#else
		std::string name(variableName.begin(), variableName.end());
		const char* value = std::getenv(name.c_str());
		if (value == nullptr || value[0] == '\0')
		{
			return std::nullopt;
		}
		return std::filesystem::path(value);
#endif
	}

}  // namespace Engine::FileSystem
