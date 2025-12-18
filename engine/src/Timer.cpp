// Timer.cpp - implementation of Timer API declared in Timer.h

#include "Timer.h"

// Define the global singleton instance
Timer gTimer;

Timer::Timer() noexcept
    : m_start(), m_last(),
      m_unscaledDelta(Rep(1.0 / 60.0)),
      m_unscaledTotal(Rep::zero()),
      m_scaledTotal(Rep::zero()),
      m_timeScale(1.0),
      m_paused(false),
      m_frameCount(0),
      m_timeInfo(TimeInfo()),
      m_initialized(false)
{
}

void Timer::Initialize()
{
    m_start = m_last = Clock::now();
    m_initialized = true;
}

void Timer::Tick()
{
    if (!m_initialized) 
    {
        Initialize();
    }

    auto now = Clock::now();
    m_unscaledDelta = std::chrono::duration_cast<Rep>(now - m_last);
    m_last = now;

    m_unscaledTotal += m_unscaledDelta;

    const bool paused = m_paused.load(std::memory_order_relaxed);
    if (!paused) {
        Rep scaled(m_unscaledDelta.count() * m_timeScale);
        m_scaledTotal += scaled;
    }

    ++m_frameCount;

    m_timeInfo.frameIndex    = m_frameCount;
    m_timeInfo.unscaledTime  = m_unscaledTotal;
    m_timeInfo.unscaledDelta = m_unscaledDelta;
    m_timeInfo.timeScale     = m_timeScale;
    m_timeInfo.scaledDelta   = paused ? Rep::zero() : Rep(m_unscaledDelta.count() * m_timeScale);
    m_timeInfo.paused        = paused;
}
