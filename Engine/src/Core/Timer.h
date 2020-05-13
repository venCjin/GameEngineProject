#pragma once

#include <chrono>

namespace sixengine {

	typedef std::chrono::high_resolution_clock HighResClock;
	typedef std::chrono::time_point<HighResClock> TimePoint;

	const double FRAMERATE_FREE = 0.0;
	const double FRAMERATE_60 = 0.016666666;
	const double FRAMERATE_30 = 0.033333333;

	const double FIXED_UPDATE = 0.01;

	const double SECOND = 1000000000.0;
	const double MILISECOND = 1000000.0;
	const double MICROSECOND = 1000.0;

	class Timer
	{
	private:
		static Timer* m_TimerInstance;

		TimePoint m_StartTime;
		TimePoint m_CurrentTime;

		double m_DeltaTime;
		double m_ElapsedTime;
		double m_ElapsedTimeUnscaled;

		float m_TimeScale;
		bool m_Paused;

		Timer();
		~Timer();
	public:
		static Timer* Instance();
		static void Release();

		static double GetTime(double unit = 1.0);
		static TimePoint GetTimePoint();

		void Tick();
		void Reset();

		bool IsPaused() const;
		void SetPaused(bool paused);

		float GetTimeScale() const;
		void SetTimeScale(float timeScale);

		double TimeSinceReset() const;

		double DeltaTime() const;
		double DeltaTimeUnscaled() const;
		double ElapsedTime() const;
		double ElapsedTimeUnscaled() const;
	};

}
