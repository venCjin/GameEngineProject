#pragma once
#include <Core/AudioManager.h>
#include <Engine.h>
#include <irrKlang.h>

namespace sixengine 
{
	class StaticLoopedSound 
	{
		std::string m_Key;
		glm::vec3 m_Position;
		float m_Rad = 10.0f;
		irrklang::ISound* m_Sound = nullptr;
	public:

		StaticLoopedSound(std::string key, glm::vec3 position) : m_Key(key), m_Position(position) {}

		StaticLoopedSound(std::string key, glm::vec3 position, float rad) : m_Key(key), m_Position(position), m_Rad(rad)
		{
			m_Sound = INIT_TRACK_3D_LOOPED(key, position);
		}

		glm::vec3 GetPosition()
		{
			return m_Position;
		}

		void SetVolume(float value)
		{
			m_Sound->setVolume(value);
		}

		void SetRad(float rad) 
		{
			this->m_Rad = rad;
			
		}

		float GetRad()
		{
			return m_Rad;
		}

		void Play() 
		{
			if (m_Sound)
				m_Sound->setIsPaused(false);
		}

		void Pause() 
		{
			if (m_Sound)
				m_Sound->setIsPaused(true);
		}


		bool isPaused()
		{
			if (m_Sound)
				return m_Sound->getIsPaused();
			else
				return false;
		}
		//StaticLoopedSound();
	};
	
}