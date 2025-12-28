#pragma once
#include <chrono>
#include <atomic>
#include <cstdint>

namespace Engine
{
	// -------------------------------------------------------------------------
	// Clock types (internal). Kept inside namespace to avoid global pollution.
	// -------------------------------------------------------------------------
	using Clock = std::chrono::steady_clock;
	using TimePoint = Clock::time_point;
	using Duration = std::chrono::duration<double>;  // seconds, double-precision

	// Supported time display units. Default is Milliseconds (convenient for UI).
	enum class TimeUnit : uint8_t
	{
		Seconds,
		Milliseconds,
		Microseconds,
		Nanoseconds
	};

	// -------------------------------------------------------------------------
	// TimeInfo: immutable snapshot of frame timing. Cheap to copy by value.
	// -------------------------------------------------------------------------
	struct TimeInfo
	{
		uint64_t frameIndex = 0;                    // 1-based frame counter
		Duration unscaledTime = Duration::zero();   // total wall time since init
		Duration unscaledDelta = Duration::zero();  // raw delta this frame (seconds)
		double timeScale = 1.0;                     // game-time multiplier
		Duration scaledDelta = Duration::zero();    // delta * timeScale (0 if paused)
		bool bPaused = false;                       // true when scaled time is paused
	};

	// -------------------------------------------------------------------------
	// Timer: authoritative frame-timing service for the engine.
	//
	// Responsibilities:
	//   - Maintain a monotonic high-precision clock.
	//   - Provide per-frame TimeInfo snapshot for subsystems.
	//   - Offer unit-aware accessors (ms, s, µs, ns) for UI/profiling.
	//
	// Usage:
	//   Call Tick() once per frame from the main loop before update/render.
	//   Query GetTimeInfo() or GetDelta() from any subsystem that needs timing.
	//
	// Thread-safety:
	//   Tick() must be called from the main thread only.
	//   Pause/Resume use relaxed atomics; safe to call from other threads.
	// -------------------------------------------------------------------------
	class Timer final
	{
	  public:
		Timer() = default;
		~Timer() = default;

		// Non-copyable, non-movable (singleton pattern).
		Timer(const Timer&) = delete;
		Timer& operator=(const Timer&) = delete;
		Timer(Timer&&) = delete;
		Timer& operator=(Timer&&) = delete;

		// Initialization. Called automatically on first Tick() if omitted.
		void Initialize() noexcept;

		// Advance clocks. Call once per rendered frame.
		void Tick() noexcept;

		// -------------------------------------------------------------------------
		// Queries
		// -------------------------------------------------------------------------

		// Immutable snapshot of current frame timing.
		[[nodiscard]] TimeInfo GetTimeInfo() const noexcept { return m_timeInfo; }

		// Raw duration accessors for advanced/internal use.
		[[nodiscard]] Duration GetDeltaRaw() const noexcept { return m_timeInfo.scaledDelta; }
		[[nodiscard]] Duration GetUnscaledDeltaRaw() const noexcept { return m_unscaledDelta; }
		[[nodiscard]] Duration GetTotalTimeRaw() const noexcept { return m_unscaledTotal; }

		// Frame counter (1-based, incremented each Tick).
		[[nodiscard]] uint64_t GetFrameCount() const noexcept { return m_frameCount; }

		// -------------------------------------------------------------------------
		// Unit-aware accessors (default: Milliseconds)
		// -------------------------------------------------------------------------

		[[nodiscard]] double GetDelta(TimeUnit unit = TimeUnit::Milliseconds) const noexcept;
		[[nodiscard]] double GetUnscaledDelta(TimeUnit unit = TimeUnit::Milliseconds) const noexcept;
		[[nodiscard]] double GetTotalTime(TimeUnit unit = TimeUnit::Milliseconds) const noexcept;

		// -------------------------------------------------------------------------
		// Time-scale controls
		// -------------------------------------------------------------------------

		void SetTimeScale(double scale) noexcept { m_timeScale = scale; }
		[[nodiscard]] double GetTimeScale() const noexcept { return m_timeScale; }

		void Pause() noexcept { m_bPaused.store(true, std::memory_order_relaxed); }
		void Resume() noexcept { m_bPaused.store(false, std::memory_order_relaxed); }
		[[nodiscard]] bool IsPaused() const noexcept { return m_bPaused.load(std::memory_order_relaxed); }

		// -------------------------------------------------------------------------
		// Stopwatch: lightweight RAII timer for profiling code sections.
		// -------------------------------------------------------------------------

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
		// Convert Duration to double in requested unit.
		[[nodiscard]] static double ToUnit(Duration d, TimeUnit u) noexcept;

	  private:
		TimePoint m_start{};
		TimePoint m_last{};
		Duration m_unscaledDelta{1.0 / 60.0};  // sensible default until first Tick
		Duration m_unscaledTotal{Duration::zero()};
		Duration m_scaledTotal{Duration::zero()};
		double m_timeScale{1.0};
		std::atomic<bool> m_bPaused{false};
		uint64_t m_frameCount{0};
		TimeInfo m_timeInfo{};
		bool m_bInitialized{false};
	};

}  // namespace Engine

// Global singleton instance. Prefer accessing via `GTimer`.
extern Engine::Timer GTimer;
