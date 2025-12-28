#include "PCH.h"
#include "D3D12SamplerLibrary.h"

#include <cassert>

const std::array<uint32_t, D3D12SamplerLibrary::kAnisoLevelCount> D3D12SamplerLibrary::kAnisoLevels = {1u, 2u, 4u, 8u, 16u};

void D3D12SamplerLibrary::Initialize()
{
	if (m_bInitialized)
	{
		return;
	}

	m_pointWrap2D.emplace(MakeSamplerDesc(FilterMode::Point, AddressMode::Wrap, Dimension::Tex2D));
	m_pointClamp2D.emplace(MakeSamplerDesc(FilterMode::Point, AddressMode::Clamp, Dimension::Tex2D));
	m_pointMirror2D.emplace(MakeSamplerDesc(FilterMode::Point, AddressMode::Mirror, Dimension::Tex2D));

	m_linearWrap2D.emplace(MakeSamplerDesc(FilterMode::Linear, AddressMode::Wrap, Dimension::Tex2D));
	m_linearClamp2D.emplace(MakeSamplerDesc(FilterMode::Linear, AddressMode::Clamp, Dimension::Tex2D));
	m_linearMirror2D.emplace(MakeSamplerDesc(FilterMode::Linear, AddressMode::Mirror, Dimension::Tex2D));

	m_pointWrap3D.emplace(MakeSamplerDesc(FilterMode::Point, AddressMode::Wrap, Dimension::Tex3D));
	m_pointClamp3D.emplace(MakeSamplerDesc(FilterMode::Point, AddressMode::Clamp, Dimension::Tex3D));
	m_pointMirror3D.emplace(MakeSamplerDesc(FilterMode::Point, AddressMode::Mirror, Dimension::Tex3D));

	m_linearWrap3D.emplace(MakeSamplerDesc(FilterMode::Linear, AddressMode::Wrap, Dimension::Tex3D));
	m_linearClamp3D.emplace(MakeSamplerDesc(FilterMode::Linear, AddressMode::Clamp, Dimension::Tex3D));
	m_linearMirror3D.emplace(MakeSamplerDesc(FilterMode::Linear, AddressMode::Mirror, Dimension::Tex3D));

	for (size_t i = 0; i < kAnisoLevelCount; ++i)
	{
		const uint32_t maxAnisotropy = kAnisoLevels[i];
		m_anisoWrap2D[i].emplace(MakeSamplerDesc(FilterMode::Anisotropic, AddressMode::Wrap, Dimension::Tex2D, maxAnisotropy));
		m_anisoClamp2D[i].emplace(MakeSamplerDesc(FilterMode::Anisotropic, AddressMode::Clamp, Dimension::Tex2D, maxAnisotropy));
		m_anisoMirror2D[i].emplace(MakeSamplerDesc(FilterMode::Anisotropic, AddressMode::Mirror, Dimension::Tex2D, maxAnisotropy));
		m_anisoWrap3D[i].emplace(MakeSamplerDesc(FilterMode::Anisotropic, AddressMode::Wrap, Dimension::Tex3D, maxAnisotropy));
		m_anisoClamp3D[i].emplace(MakeSamplerDesc(FilterMode::Anisotropic, AddressMode::Clamp, Dimension::Tex3D, maxAnisotropy));
		m_anisoMirror3D[i].emplace(MakeSamplerDesc(FilterMode::Anisotropic, AddressMode::Mirror, Dimension::Tex3D, maxAnisotropy));
	}

	m_bInitialized = true;
}

void D3D12SamplerLibrary::Shutdown() noexcept
{
	for (auto& sampler : m_anisoMirror3D)
	{
		sampler.reset();
	}
	for (auto& sampler : m_anisoClamp3D)
	{
		sampler.reset();
	}
	for (auto& sampler : m_anisoWrap3D)
	{
		sampler.reset();
	}
	for (auto& sampler : m_anisoMirror2D)
	{
		sampler.reset();
	}
	for (auto& sampler : m_anisoClamp2D)
	{
		sampler.reset();
	}
	for (auto& sampler : m_anisoWrap2D)
	{
		sampler.reset();
	}

	m_linearMirror3D.reset();
	m_linearClamp3D.reset();
	m_linearWrap3D.reset();
	m_pointMirror3D.reset();
	m_pointClamp3D.reset();
	m_pointWrap3D.reset();

	m_linearMirror2D.reset();
	m_linearClamp2D.reset();
	m_linearWrap2D.reset();
	m_pointMirror2D.reset();
	m_pointClamp2D.reset();
	m_pointWrap2D.reset();

	m_bInitialized = false;
}

const D3D12Sampler& D3D12SamplerLibrary::GetPoint(AddressMode addressMode, Dimension dimension) const
{
	assert(m_bInitialized);

	if (dimension == Dimension::Tex2D)
	{
		switch (addressMode)
		{
			case AddressMode::Wrap:
				return *m_pointWrap2D;
			case AddressMode::Clamp:
				return *m_pointClamp2D;
			case AddressMode::Mirror:
				return *m_pointMirror2D;
		}
	}
	else
	{
		switch (addressMode)
		{
			case AddressMode::Wrap:
				return *m_pointWrap3D;
			case AddressMode::Clamp:
				return *m_pointClamp3D;
			case AddressMode::Mirror:
				return *m_pointMirror3D;
		}
	}

	assert(false);
	return *m_pointWrap2D;
}

const D3D12Sampler& D3D12SamplerLibrary::GetLinear(AddressMode addressMode, Dimension dimension) const
{
	assert(m_bInitialized);

	if (dimension == Dimension::Tex2D)
	{
		switch (addressMode)
		{
			case AddressMode::Wrap:
				return *m_linearWrap2D;
			case AddressMode::Clamp:
				return *m_linearClamp2D;
			case AddressMode::Mirror:
				return *m_linearMirror2D;
		}
	}
	else
	{
		switch (addressMode)
		{
			case AddressMode::Wrap:
				return *m_linearWrap3D;
			case AddressMode::Clamp:
				return *m_linearClamp3D;
			case AddressMode::Mirror:
				return *m_linearMirror3D;
		}
	}

	assert(false);
	return *m_linearWrap2D;
}

const D3D12Sampler& D3D12SamplerLibrary::GetAnisotropic(AddressMode addressMode, Dimension dimension, AnisotropyLevel level) const
{
	assert(m_bInitialized);

	const uint32_t idx = GetAnisoIndex(level);
	assert(idx < kAnisoLevelCount);

	if (dimension == Dimension::Tex2D)
	{
		switch (addressMode)
		{
			case AddressMode::Wrap:
				return *m_anisoWrap2D[idx];
			case AddressMode::Clamp:
				return *m_anisoClamp2D[idx];
			case AddressMode::Mirror:
				return *m_anisoMirror2D[idx];
		}
	}
	else
	{
		switch (addressMode)
		{
			case AddressMode::Wrap:
				return *m_anisoWrap3D[idx];
			case AddressMode::Clamp:
				return *m_anisoClamp3D[idx];
			case AddressMode::Mirror:
				return *m_anisoMirror3D[idx];
		}
	}

	assert(false);
	return *m_anisoWrap2D[0];
}

uint32_t D3D12SamplerLibrary::GetAnisoIndex(AnisotropyLevel level)
{
	const uint32_t value = static_cast<uint32_t>(level);

	for (size_t i = 0; i < kAnisoLevelCount; ++i)
	{
		if (kAnisoLevels[i] == value)
		{
			return static_cast<uint32_t>(i);
		}
	}

	assert(false && "Unsupported anisotropy level");
	return 0;
}

D3D12_SAMPLER_DESC
D3D12SamplerLibrary::MakeSamplerDesc(FilterMode filterMode, AddressMode addressMode, Dimension dimension, uint32_t maxAnisotropy)
{
	D3D12_SAMPLER_DESC desc = {};
	switch (filterMode)
	{
		case FilterMode::Point:
			desc.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
			desc.MaxAnisotropy = 1;
			break;
		case FilterMode::Linear:
			desc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
			desc.MaxAnisotropy = 1;
			break;
		case FilterMode::Anisotropic:
			desc.Filter = D3D12_FILTER_ANISOTROPIC;
			desc.MaxAnisotropy = static_cast<UINT>(maxAnisotropy);
			break;
	}

	desc.AddressU = ToD3D12AddressMode(addressMode);
	desc.AddressV = ToD3D12AddressMode(addressMode);
	desc.AddressW = (dimension == Dimension::Tex3D) ? ToD3D12AddressMode(addressMode) : D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	desc.MipLODBias = 0.0f;
	desc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	desc.MinLOD = 0.0f;
	desc.MaxLOD = D3D12_FLOAT32_MAX;
	return desc;
}

D3D12_TEXTURE_ADDRESS_MODE D3D12SamplerLibrary::ToD3D12AddressMode(AddressMode addressMode)
{
	switch (addressMode)
	{
		case AddressMode::Wrap:
			return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		case AddressMode::Clamp:
			return D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		case AddressMode::Mirror:
			return D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
	}

	return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
}
