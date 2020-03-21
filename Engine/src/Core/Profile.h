#pragma once
#pragma once

#include "iostream"
#include "Timer.h"

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
			std::cout << duration << " ms " << m_Name << "\n";
		}
	};

}

#define PROFILE_SCOPE(name) ::sixengine::Profile timer##__LINE__(name);

