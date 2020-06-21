#pragma once
#include <Core/AudioManager.h>
#include <Engine.h>
#include <irrKlang.h>

namespace sixengine 
{

	class LoopedSound
	{
		std::string m_Key;
		irrklang::ISound* m_Sound = nullptr;
		//float volume;
	public:
		LoopedSound(std::string key) : m_Key(key)
		{
			m_Sound = INIT_TRACK_LOOPED(key);
		}
		void SetVolume(float value)
		{
			m_Sound->setVolume(value);
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

		bool IsPaused()
		{
			return m_Sound->getIsPaused();
		}
	};
	class LoopedSound3D 
	{
		std::string m_Key;
		glm::vec3 m_Position;
		float m_Rad = 10.0f;
		irrklang::ISound* m_Sound = nullptr;
	public:

		LoopedSound3D(std::string key, glm::vec3 position) : m_Key(key), m_Position(position) {}

		LoopedSound3D(std::string key, glm::vec3 position, float rad) : m_Key(key), m_Position(position), m_Rad(rad)
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

		void SetMinDistance(float value)
		{
			m_Sound->setMinDistance(value);
		}

		void SetPosition(glm::vec3 pos) {
			this->m_Position = pos;
			if (m_Sound) m_Sound->setPosition(irrklang::vec3df(pos.x, pos.y, pos.z));
		}

		void SetSpeed(float value)
		{
			m_Sound->setPlaybackSpeed(value);
		}
		bool isPaused()
		{
			if (m_Sound)
				return m_Sound->getIsPaused();
			else
				return false;
		}
		//LoopedSound3D();
	};
	
}