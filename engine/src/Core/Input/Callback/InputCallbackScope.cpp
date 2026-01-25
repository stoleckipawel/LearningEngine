// =============================================================================
// InputCallbackScope.cpp — Callback Scope Implementation
// =============================================================================

#include "InputCallbackScope.h"

#include <utility>  // std::move

namespace Input
{

InputCallbackScope::~InputCallbackScope()
{
	Clear();
}

InputCallbackScope::InputCallbackScope(InputCallbackScope&& other) noexcept
    : m_handles(std::move(other.m_handles))
{
}

InputCallbackScope& InputCallbackScope::operator=(InputCallbackScope&& other) noexcept
{
	if (this != &other)
	{
		Clear();
		m_handles = std::move(other.m_handles);
	}
	return *this;
}

void InputCallbackScope::Add(InputCallbackHandle&& handle)
{
	if (handle.IsValid())
	{
		m_handles.push_back(std::move(handle));
	}
}

void InputCallbackScope::Clear() noexcept
{
	// Destructor of each handle will call Unsubscribe
	m_handles.clear();
}

std::size_t InputCallbackScope::Size() const noexcept
{
	return m_handles.size();
}

bool InputCallbackScope::IsEmpty() const noexcept
{
	return m_handles.empty();
}

void InputCallbackScope::Reserve(std::size_t capacity)
{
	m_handles.reserve(capacity);
}

}  // namespace Input
