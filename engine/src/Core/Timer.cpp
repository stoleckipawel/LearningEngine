#include "Timer.h"

using namespace Engine;

// -----------------------------------------------------------------------------
// Global singleton instance
// -----------------------------------------------------------------------------
Engine::Timer GTimer;

// -----------------------------------------------------------------------------
// Initialization
// -----------------------------------------------------------------------------
void Timer::Initialize() noexcept
{
	m_start = Clock::now();
	m_last = m_start;
	m_bInitialized = true;
}

// -----------------------------------------------------------------------------
// Tick — call once per frame from the main loop
// -----------------------------------------------------------------------------
void Timer::Tick() noexcept
{
	// Sample current time and compute raw delta.
	const TimePoint now = Clock::now();
	m_unscaledDelta = std::chrono::duration_cast<Duration>(now - m_last);
	m_last = now;

	// Accumulate totals.
	m_unscaledTotal += m_unscaledDelta;

	const bool paused = m_bPaused.load(std::memory_order_relaxed);
	if (!paused)
	{
		const Duration scaled{m_unscaledDelta.count() * m_timeScale};
		m_scaledTotal += scaled;
	}

	// Advance frame counter.
	++m_frameCount;

	// Update snapshot for consumers.
	m_timeInfo.frameIndex = m_frameCount;
	m_timeInfo.unscaledTime = m_unscaledTotal;
	m_timeInfo.unscaledDelta = m_unscaledDelta;
	m_timeInfo.timeScale = m_timeScale;
	m_timeInfo.scaledDelta = paused ? Duration::zero() : Duration{m_unscaledDelta.count() * m_timeScale};
	m_timeInfo.paused = paused;
}

// -----------------------------------------------------------------------------
// Unit conversion (private helper, called by public accessors)
// -----------------------------------------------------------------------------
double Timer::ToUnit(Duration d, TimeUnit u) noexcept
{
	switch (u)
	{
	case TimeUnit::Seconds:
		return d.count();
	case TimeUnit::Milliseconds:
		return d.count() * 1e3;
	case TimeUnit::Microseconds:
		return d.count() * 1e6;
	case TimeUnit::Nanoseconds:
		return d.count() * 1e9;
	}
	return d.count(); // fallback (should never reach)
}

// -----------------------------------------------------------------------------
// Unit-aware accessors
// -----------------------------------------------------------------------------
double Timer::GetDelta(TimeUnit unit) const noexcept
{
	return ToUnit(m_timeInfo.scaledDelta, unit);
}

double Timer::GetUnscaledDelta(TimeUnit unit) const noexcept
{
	return ToUnit(m_unscaledDelta, unit);
}

double Timer::GetTotalTime(TimeUnit unit) const noexcept
{
	return ToUnit(m_unscaledTotal, unit);
}
