#pragma once

#include "D3D12Sampler.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>

// D3D12SamplerLibrary
// - Owns a predefined set of commonly used sampler states.
// - Samplers are created once during renderer/RHI initialization and reused.
// - Each sampler is a distinct allocation in the sampler heap.
class D3D12SamplerLibrary
{
  public:
	enum class AddressMode : uint8_t
	{
		Wrap,
		Clamp,
		Mirror,
	};

	enum class Dimension : uint8_t
	{
		Tex2D,
		Tex3D,
	};

	enum class AnisotropyLevel : uint8_t
	{
		x1 = 1,
		x2 = 2,
		x4 = 4,
		x8 = 8,
		x16 = 16,
	};

	D3D12SamplerLibrary() = default;
	~D3D12SamplerLibrary() noexcept = default;

	D3D12SamplerLibrary(const D3D12SamplerLibrary&) = delete;
	D3D12SamplerLibrary& operator=(const D3D12SamplerLibrary&) = delete;
	D3D12SamplerLibrary(D3D12SamplerLibrary&&) = delete;
	D3D12SamplerLibrary& operator=(D3D12SamplerLibrary&&) = delete;

	void Initialize();
	void Shutdown() noexcept;
	bool IsInitialized() const noexcept { return m_bInitialized; }

	const D3D12Sampler& GetPoint(AddressMode addressMode, Dimension dimension) const;
	const D3D12Sampler& GetLinear(AddressMode addressMode, Dimension dimension) const;
	const D3D12Sampler& GetAnisotropic(AddressMode addressMode, Dimension dimension, AnisotropyLevel level) const;

  private:
	enum class FilterMode : uint8_t
	{
		Point,
		Linear,
		Anisotropic,
	};

	static constexpr size_t kAnisoLevelCount = 5;
	static const std::array<uint32_t, kAnisoLevelCount> kAnisoLevels;

	static uint32_t GetAnisoIndex(AnisotropyLevel level);

	static D3D12_SAMPLER_DESC
	MakeSamplerDesc(FilterMode filterMode, AddressMode addressMode, Dimension dimension, uint32_t maxAnisotropy = 1);

	static D3D12_TEXTURE_ADDRESS_MODE ToD3D12AddressMode(AddressMode addressMode);

  private:
	bool m_bInitialized = false;

	std::optional<D3D12Sampler> m_pointWrap2D;
	std::optional<D3D12Sampler> m_pointClamp2D;
	std::optional<D3D12Sampler> m_pointMirror2D;

	std::optional<D3D12Sampler> m_linearWrap2D;
	std::optional<D3D12Sampler> m_linearClamp2D;
	std::optional<D3D12Sampler> m_linearMirror2D;

	std::optional<D3D12Sampler> m_pointWrap3D;
	std::optional<D3D12Sampler> m_pointClamp3D;
	std::optional<D3D12Sampler> m_pointMirror3D;

	std::optional<D3D12Sampler> m_linearWrap3D;
	std::optional<D3D12Sampler> m_linearClamp3D;
	std::optional<D3D12Sampler> m_linearMirror3D;

	std::array<std::optional<D3D12Sampler>, kAnisoLevelCount> m_anisoWrap2D;
	std::array<std::optional<D3D12Sampler>, kAnisoLevelCount> m_anisoClamp2D;
	std::array<std::optional<D3D12Sampler>, kAnisoLevelCount> m_anisoMirror2D;
	std::array<std::optional<D3D12Sampler>, kAnisoLevelCount> m_anisoWrap3D;
	std::array<std::optional<D3D12Sampler>, kAnisoLevelCount> m_anisoClamp3D;
	std::array<std::optional<D3D12Sampler>, kAnisoLevelCount> m_anisoMirror3D;
};
