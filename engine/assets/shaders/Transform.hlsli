#pragma once

#include "ConstantBufferData.hlsli"

float4 PositionLocalToWorld(float4 LocalPosition)
{
	return mul(LocalPosition, WorldMTX);
}

float4 PositionWorldToView(float4 WorldPosition)
{
	return mul(WorldPosition, ViewMTX);
}

float4 PositionViewToClip(float4 ViewPosition)
{
	return mul(ViewPosition, ProjectionMTX);
}

float4 PositionLocalToClip(float4 LocalPosition)
{
	const float4 worldPosition = PositionLocalToWorld(LocalPosition);
	const float4 viewPosition = PositionWorldToView(worldPosition);
	return PositionViewToClip(viewPosition);
}