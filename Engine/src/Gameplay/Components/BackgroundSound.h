#pragma once
#include <Engine.h>
#include <irrKlang.h>
namespace sixengine 
{
	class BackgroundSound
	{
	public:
		float minInterval, maxInterval, interval = 0.0f;
		float timer = 0.0f;
		std::string m_Key;
		irrklang::ISound* m_Sound = nullptr;
		BackgroundSound(float minInterval, float maxInterval, std::string key) : minInterval(minInterval), maxInterval(maxInterval), m_Key(key)
		{
			timer = 0.0f;
			interval = 0.0f;
		}
	};
}