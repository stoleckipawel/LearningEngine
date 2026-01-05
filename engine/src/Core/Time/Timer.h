// ============================================================================
// Timer.h
// ----------------------------------------------------------------------------
// Authoritative frame-timing service for the engine.
//
// USAGE:
//   Engine::GTimer.Initialize();
//   // Each frame:
//   Engine::GTimer.Tick();
//   auto info = Engine::GTimer.GetTimeInfo();
//   double dt = Engine::GTimer.GetDelta(TimeDomain::Scaled);
//
// DESIGN:
//   - Provides both unscaled (wall) and scaled (game) time domains
//   - TimeInfo struct gives immutable snapshot of frame timing
//   - Supports pause/resume and time scaling for slow-mo effects
//
// NOTES:
//   - Singleton accessed via Engine::GTimer global reference
//   - Initialize() called automatically on first Tick() if omitted
//   - Frame counter is 1-based
// ============================================================================

#pragma once

#include <atomic>
#include <chrono>
#include <cstdint>

namespace Engine
{
	// ========================================================================
	// Internal Clock Types
	// ========================================================================

	using Clock = std::chrono::steady_clock;
	using TimePoint = Clock::time_point;
	using Duration = std::chrono::duration<double>;  ///< Seconds, double-precision

	// Supported time display units. Default is Milliseconds (convenient for UI).
	enum class TimeUnit : uint8_t
	{
		Seconds,
		Milliseconds,
		Microseconds,
		Nanoseconds
	};

	// Which clock domain to query.
	enum class TimeDomain : uint8_t
	{
		Unscaled,  // wall/real time (ignores timeScale, continues when paused)
		Scaled     // game time (multiplied by timeScale, stops when paused)
	};

	// -------------------------------------------------------------------------
	// TimeInfo: immutable snapshot of frame timing. Cheap to copy by value.
	// -------------------------------------------------------------------------
	struct TimeInfo
	{
		uint64_t frameIndex = 0;                    // 1-based frame counter
		Duration unscaledTime = Duration::zero();   // total wall time since init
		Duration scaledTime = Duration::zero();     // total scaled/game time since init (stops when paused)
		Duration unscaledDelta = Duration::zero();  // raw delta this frame (seconds)
		double timeScale = 1.0;                     // game-time multiplier
		Duration scaledDelta = Duration::zero();    // delta * timeScale (0 if paused)
		bool bPaused = false;                       // true when scaled time is paused
	};

	// -------------------------------------------------------------------------
	// Timer: authoritative frame-timing service for the engine.
	// -------------------------------------------------------------------------
	class Timer final
	{
	  public:
		[[nodiscard]] static Timer& Get() noexcept;

		Timer(const Timer&) = delete;
		Timer& operator=(const Timer&) = delete;
		Timer(Timer&&) = delete;
		Timer& operator=(Timer&&) = delete;

		// Initialization. Called automatically on first Tick() if omitted.
		void Initialize() noexcept;

		// Advance clocks. Call once per rendered frame.
		void Tick() noexcept;

		// Immutable snapshot of current frame timing.
		[[nodiscard]] TimeInfo GetTimeInfo() const noexcept { return m_timeInfo; }

		// Frame counter (1-based, incremented each Tick).
		[[nodiscard]] uint64_t GetFrameCount() const noexcept { return m_frameCount; }

		[[nodiscard]] double GetDelta(TimeDomain domain, TimeUnit unit = TimeUnit::Milliseconds) const noexcept;
		[[nodiscard]] double GetTotalTime(TimeDomain domain, TimeUnit unit = TimeUnit::Milliseconds) const noexcept;

		void SetTimeScale(double scale) noexcept { m_timeScale = scale; }
		[[nodiscard]] double GetTimeScale() const noexcept { return m_timeScale; }

		void Pause() noexcept { m_bPaused.store(true, std::memory_order_relaxed); }
		void Resume() noexcept { m_bPaused.store(false, std::memory_order_relaxed); }
		[[nodiscard]] bool IsPaused() const noexcept { return m_bPaused.load(std::memory_order_relaxed); }

		struct Stopwatch
		{
			TimePoint start;

			Stopwatch() noexcept : start(Clock::now()) {}
			void Reset() noexcept { start = Clock::now(); }

			[[nodiscard]] Duration Elapsed() const noexcept { return std::chrono::duration_cast<Duration>(Clock::now() - start); }
			[[nodiscard]] double ElapsedSeconds() const noexcept { return Elapsed().count(); }
			[[nodiscard]] double ElapsedMillis() const noexcept { return Elapsed().count() * 1e3; }
		};

	  private:
		Timer() = default;
		~Timer() = default;

		[[nodiscard]] static double ToUnit(Duration d, TimeUnit u) noexcept;

		TimePoint m_start{};
		TimePoint m_last{};
		Duration m_unscaledDelta{1.0 / 60.0};
		Duration m_unscaledTotal{Duration::zero()};
		Duration m_scaledTotal{Duration::zero()};
		double m_timeScale{1.0};
		std::atomic<bool> m_bPaused{false};
		uint64_t m_frameCount{0};
		TimeInfo m_timeInfo{};
		bool m_bInitialized{false};
	};

}  // namespace Engine

inline Engine::Timer& GTimer = Engine::Timer::Get();
