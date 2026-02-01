// ============================================================================
// EventHandle.h
// ----------------------------------------------------------------------------
// Opaque handle for event subscription management.
//
// USAGE:
//   EventHandle handle = event.Add([](){ ... });
//   if (handle.IsValid()) { ... }
//   event.Remove(handle);
//
// ============================================================================

#pragma once

#include <cstdint>

// ============================================================================
// EventHandle
// ============================================================================

/// Opaque handle returned by Event::Add(), used for Event::Remove().
/// A handle with Id == 0 is considered invalid.
struct EventHandle
{
	std::uint32_t Id = 0;  ///< Unique identifier (0 = invalid)

	/// Returns true if handle points to a valid subscription.
	[[nodiscard]] bool IsValid() const noexcept { return Id != 0; }

	/// Marks this handle as invalid.
	void Invalidate() noexcept { Id = 0; }

	[[nodiscard]] bool operator==(const EventHandle& Other) const noexcept { return Id == Other.Id; }
	[[nodiscard]] bool operator!=(const EventHandle& Other) const noexcept { return Id != Other.Id; }
};
