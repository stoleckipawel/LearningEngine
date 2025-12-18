#include <chrono>
#include <atomic>
#include <cstdint>

// Primary clock types and aliases (still accessible from the namespace)
using Clock     = std::chrono::steady_clock;       // monotonic clock
using TimePoint = Clock::time_point;               // point in time from Clock
using Rep  = std::chrono::duration<double>;   // seconds (double precision)

// Units supported by the API. Default is Milliseconds for convenient UI/workers.
enum class TimeUnit : uint8_t { Seconds, Milliseconds, Microseconds, Nanoseconds };

// Convert a Rep duration to a double value in the requested unit.
static inline double RepToUnit(const Rep &d, TimeUnit u) noexcept
{
    switch (u) 
    {
        case TimeUnit::Seconds:     return d.count();
        case TimeUnit::Milliseconds:return d.count() * 1e3;
        case TimeUnit::Microseconds:return d.count() * 1e6;
        case TimeUnit::Nanoseconds: return d.count() * 1e9;
        default: return d.count();
    }
}

// FrameInfo is a compact snapshot describing the current frame timing.
struct TimeInfo
{
    uint64_t frameIndex = 0;        // 1-based frame counter
    Rep unscaledTime  = Rep::zero(); // total wall time since Initialize
    Rep unscaledDelta = Rep::zero(); // raw delta for this frame (seconds)
    double   timeScale     = 1.0;   // game time scale multiplier
    Rep scaledDelta   = Rep::zero(); // unscaledDelta * timeScale (0 if paused)
    bool     paused = false;        // true if scaled time is paused
};

// Timer: singleton-style class (instance exposed as `gTimer`).
class Timer
{
public:
    Timer() noexcept;
    // Initialization / per-frame
    void Initialize();
    void Tick();

    // Queries
    TimeInfo GetTimeInfo() const { return m_timeInfo; }
    // Raw duration accessors (Rep) for low-level consumers
    Rep GetDeltaRaw() const { return m_timeInfo.scaledDelta; }
    Rep GetUnscaledDeltaRaw() const { return m_unscaledDelta; }
    uint64_t GetFrameCount() const { return m_frameCount; }    

    // Unified accessors returning a double in requested unit (default ms)
    double GetDelta(TimeUnit unit = TimeUnit::Milliseconds) const { return RepToUnit(GetDeltaRaw(), unit); }
    double GetUnscaledDelta(TimeUnit unit = TimeUnit::Milliseconds) const { return RepToUnit(GetUnscaledDeltaRaw(), unit); }
    double GetTotalTime(TimeUnit unit = TimeUnit::Milliseconds) const { return RepToUnit(m_unscaledTotal, unit); }

    // Controls
    void SetTimeScale(double s) { m_timeScale = s; }
    double GetTimeScale() const { return m_timeScale; }
    void Pause() { m_paused = true; }
    void Resume() { m_paused = false; }
    bool IsPaused() const { return m_paused.load(std::memory_order_relaxed); }

    // Stopwatch (nested for convenience)
    struct Stopwatch
    {
        TimePoint start;
        Stopwatch::Stopwatch() : start(Clock::now()) {}
        void Stopwatch::Reset() { start = Clock::now(); }
        Rep Stopwatch::Elapsed() const { return std::chrono::duration_cast<Rep>(Clock::now() - start); }
        double Stopwatch::ElapsedSeconds() const { return Elapsed().count(); }
        double Stopwatch::ElapsedMillis() const { return Elapsed().count() * 1000.0; }
    };
private:
    TimePoint m_start;
    TimePoint m_last;
    Rep  m_unscaledDelta;
    Rep  m_unscaledTotal;
    Rep  m_scaledTotal;
    double    m_timeScale;
    std::atomic<bool> m_paused;
    uint64_t  m_frameCount;
    TimeInfo m_timeInfo;
    std::atomic<bool> m_initialized;
};

// Global singleton instance
extern Timer gTimer;






