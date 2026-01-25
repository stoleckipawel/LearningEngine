// =============================================================================
// InputCallbackHandle.cpp — RAII Subscription Handle Implementation
// =============================================================================

#include "InputCallbackHandle.h"

#include <utility>  // std::move, std::exchange

namespace Input
{

// =============================================================================
// InputCallbackHandle Implementation
// =============================================================================

InputCallbackHandle::InputCallbackHandle(std::uint64_t id, UnsubscribeFunc unsubscribeFunc) noexcept
    : m_id(id)
    , m_unsubscribeFunc(std::move(unsubscribeFunc))
{
}

InputCallbackHandle::~InputCallbackHandle()
{
	Unsubscribe();
}

InputCallbackHandle::InputCallbackHandle(InputCallbackHandle&& other) noexcept
    : m_id(std::exchange(other.m_id, 0))
    , m_unsubscribeFunc(std::move(other.m_unsubscribeFunc))
{
}

InputCallbackHandle& InputCallbackHandle::operator=(InputCallbackHandle&& other) noexcept
{
	if (this != &other)
	{
		// Unsubscribe current before taking new
		Unsubscribe();

		m_id = std::exchange(other.m_id, 0);
		m_unsubscribeFunc = std::move(other.m_unsubscribeFunc);
	}
	return *this;
}

void InputCallbackHandle::Unsubscribe() noexcept
{
	if (m_id != 0 && m_unsubscribeFunc)
	{
		m_unsubscribeFunc();
		m_id = 0;
		m_unsubscribeFunc = nullptr;
	}
}

bool InputCallbackHandle::IsValid() const noexcept
{
	return m_id != 0;
}

InputCallbackHandle::operator bool() const noexcept
{
	return IsValid();
}

std::uint64_t InputCallbackHandle::GetId() const noexcept
{
	return m_id;
}

std::uint64_t InputCallbackHandle::Release() noexcept
{
	m_unsubscribeFunc = nullptr;
	return std::exchange(m_id, 0);
}

}  // namespace Input
{
	return m_handles.empty();
}

void InputCallbackScope::Reserve(std::size_t capacity)
{
	m_handles.reserve(capacity);
}

}  // namespace Input
