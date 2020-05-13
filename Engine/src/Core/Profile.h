#pragma once
#pragma once

#include "iostream"
#include "Timer.h"

#define PROFILE_SWITCH true

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
			if (duration > 17)
				LOG_CORE_WARN("Profile: {0:.3f} ms {1}", duration, m_Name);
			else
				LOG_CORE_INFO("Profile: {0:.3f} ms {1}", duration, m_Name);
		}
	};

}

#if PROFILE_SWITCH
	#define PROFILE_SCOPE(name) ::sixengine::Profile timer##__LINE__(name);
#else
	#define PROFILE_SCOPE(name)
#endif // PROFILE_SWITCH


