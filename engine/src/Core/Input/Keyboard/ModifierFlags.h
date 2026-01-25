// =============================================================================
// ModifierFlags.h — Keyboard Modifier Bitmask
// =============================================================================
//
// Bitmask flags for modifier key state. Queryable alongside any input event
// to check for key combinations.
//
// =============================================================================

#pragma once

#include <cstdint>
#include <type_traits>

namespace Input
{

// =============================================================================
// ModifierFlags — Keyboard Modifier Bitmask
// =============================================================================

enum class ModifierFlags : std::uint8_t
{
	None = 0,
	Shift = 1 << 0,
	Ctrl = 1 << 1,
	Alt = 1 << 2,
	Super = 1 << 3,  // Windows key / Command

	// Common combinations
	CtrlShift = Ctrl | Shift,
	CtrlAlt = Ctrl | Alt,
	ShiftAlt = Shift | Alt,
	All = Shift | Ctrl | Alt | Super
};

// =============================================================================
// Bitwise Operators
// =============================================================================

[[nodiscard]] constexpr ModifierFlags operator|(ModifierFlags lhs, ModifierFlags rhs) noexcept
{
	return static_cast<ModifierFlags>(
	    static_cast<std::underlying_type_t<ModifierFlags>>(lhs) | static_cast<std::underlying_type_t<ModifierFlags>>(rhs)
	);
}

[[nodiscard]] constexpr ModifierFlags operator&(ModifierFlags lhs, ModifierFlags rhs) noexcept
{
	return static_cast<ModifierFlags>(
	    static_cast<std::underlying_type_t<ModifierFlags>>(lhs) & static_cast<std::underlying_type_t<ModifierFlags>>(rhs)
	);
}

[[nodiscard]] constexpr ModifierFlags operator^(ModifierFlags lhs, ModifierFlags rhs) noexcept
{
	return static_cast<ModifierFlags>(
	    static_cast<std::underlying_type_t<ModifierFlags>>(lhs) ^ static_cast<std::underlying_type_t<ModifierFlags>>(rhs)
	);
}

[[nodiscard]] constexpr ModifierFlags operator~(ModifierFlags flags) noexcept
{
	return static_cast<ModifierFlags>(~static_cast<std::underlying_type_t<ModifierFlags>>(flags));
}

constexpr ModifierFlags& operator|=(ModifierFlags& lhs, ModifierFlags rhs) noexcept
{
	lhs = lhs | rhs;
	return lhs;
}

constexpr ModifierFlags& operator&=(ModifierFlags& lhs, ModifierFlags rhs) noexcept
{
	lhs = lhs & rhs;
	return lhs;
}

constexpr ModifierFlags& operator^=(ModifierFlags& lhs, ModifierFlags rhs) noexcept
{
	lhs = lhs ^ rhs;
	return lhs;
}

// =============================================================================
// ModifierFlags Utilities
// =============================================================================

/// Checks if all specified modifier flags are set.
[[nodiscard]] constexpr bool HasAllFlags(ModifierFlags flags, ModifierFlags test) noexcept
{
	return (flags & test) == test;
}

/// Checks if any of the specified modifier flags are set.
[[nodiscard]] constexpr bool HasAnyFlag(ModifierFlags flags, ModifierFlags test) noexcept
{
	return (flags & test) != ModifierFlags::None;
}

}  // namespace Input
