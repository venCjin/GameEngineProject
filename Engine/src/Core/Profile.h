#pragma once
#pragma once

#include "Timer.h"
#include "Log.h"

namespace sixengine {

	class Profile
	{
	private:
		const char* m_Name;
		double m_Start;

	public:
		Profile(const char* name)
			: m_Name(name)
		{
			m_Start = Timer::GetTime(MILISECOND);
		}

		~Profile()
		{
			double duration = Timer::GetTime(MILISECOND) - m_Start;
			LOG_CORE_INFO("Profile: {0:.3f} ms {1}", duration, m_Name);
		}
	};

}

#define PROFILE_SCOPE(name) ::sixengine::Profile timer##__LINE__(name);

