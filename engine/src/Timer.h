#include <chrono>
#include <atomic>
#include <cstdint>

// Primary clock types and aliases (still accessible from the namespace)
using Clock     = std::chrono::steady_clock;       // monotonic clock
using TimePoint = Clock::time_point;               // point in time from Clock
using Rep  = std::chrono::duration<double>;   // seconds (double precision)

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
    Rep GetDelta() const { return m_timeInfo.scaledDelta; }
    Rep GetUnscaledDelta() const { return m_unscaledDelta; }
    uint64_t GetFrameCount() const { return m_frameCount; }    

    // Conversions
    double GetDeltaSeconds() const { return GetDelta().count(); }
    double GetUnscaledDeltaSeconds() const { return GetUnscaledDelta().count(); }
    double GetDeltaMillis() const { return GetDelta().count() * 1000.0; }
    double GetUnscaledDeltaMillis() const { return GetUnscaledDelta().count() * 1000.0; }
    double GetTotalTimeSeconds() const { return m_unscaledTotal.count(); }
    double GetTotalTimeMillis() const { return m_unscaledTotal.count() * 1000.0; }

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






