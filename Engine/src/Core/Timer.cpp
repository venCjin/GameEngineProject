#include "pch.h"
#include "Timer.h"

namespace sixengine {

	Timer* Timer::m_TimerInstance = nullptr;

	Timer::Timer()
		: m_StartTime(HighResClock::now()),
		m_CurrentTime(HighResClock::now()),
		m_DeltaTime(0.f),
		m_ElapsedTime(0.f),
		m_ElapsedTimeUnscaled(0.f),
		m_TimeScale(1.f),
		m_Paused(false),
		m_IsFixedDeltaTime(false),
		m_FixedDeltaTime(FRAMERATE_60)
	{
		Reset();
	}

	Timer::~Timer()
	{
	}

	Timer* Timer::Instance()
	{
		if (!m_TimerInstance)
			m_TimerInstance = new Timer();

		return m_TimerInstance;
	}

	void Timer::Release()
	{
		delete m_TimerInstance;
		m_TimerInstance = nullptr;
	}

	double Timer::GetTime(double unit)
	{
		return std::chrono::duration_cast<std::chrono::nanoseconds>(HighResClock::now().time_since_epoch()).count() / unit;
	}

	TimePoint Timer::GetTimePoint()
	{
		return HighResClock::now();
	}

	void Timer::Tick()
	{
		if (m_Paused)
		{
			m_DeltaTime = 0.f;
			return;
		}

		if (m_IsFixedDeltaTime)
			m_DeltaTime = m_FixedDeltaTime;
		else
			m_DeltaTime = (HighResClock::now() - m_CurrentTime).count() / SECOND;

		m_ElapsedTime += m_DeltaTime * m_TimeScale;
		m_ElapsedTimeUnscaled += m_DeltaTime;
	}

	void Timer::Reset()
	{
		m_CurrentTime = HighResClock::now();
	}

	void Timer::SetPaused(bool paused)
	{
		m_Paused = paused;

		if (!m_Paused)
		{
			m_CurrentTime = HighResClock::now();
		}
	}

	void Timer::SetIsFixedUpdate(bool fixed) { m_IsFixedDeltaTime = fixed; }
	void Timer::SetFixedUpdate(float fixed) { m_FixedDeltaTime = fixed; }

	bool Timer::IsPaused() const { return m_Paused; }

	void Timer::SetTimeScale(float timeScale) { m_TimeScale = timeScale; }
	float Timer::GetTimeScale() const { return m_TimeScale; }

	double Timer::TimeSinceReset() const { return (HighResClock::now() - m_CurrentTime).count() / SECOND; }

	double Timer::DeltaTime() const { return m_DeltaTime * m_TimeScale; }
	double Timer::DeltaTimeUnscaled() const { return m_DeltaTime; }

	double Timer::ElapsedTime() const { return m_ElapsedTime; }
	double Timer::ElapsedTimeUnscaled() const { return m_ElapsedTimeUnscaled; }

}
