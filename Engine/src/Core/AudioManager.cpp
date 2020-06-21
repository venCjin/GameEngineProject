#include "pch.h"
#include "AudioManager.h"

namespace sixengine
{

	AudioManager* AudioManager::instance = nullptr;
	AudioManager::AudioManager() : ResourceManager<std::string>()
	{
		this->m_SoundEngine = irrklang::createIrrKlangDevice();
		this->sounds = std::vector<irrklang::ISound*>();
	}

	AudioManager* AudioManager::getInstance()
	{
		if (instance == nullptr)
		{
			instance = new AudioManager();
		}
		return instance;
	}

	irrklang::ISound* AudioManager::Init(std::string key)
	{
		irrklang::ISound* s = m_SoundEngine->play2D(Get(key).c_str(),
			false,
			true,
			true);
		sounds.push_back(s);
		//s->setIsPaused(false);
		return s;
	}

	irrklang::ISound* AudioManager::InitLooped(std::string key)
	{
		irrklang::ISound* s = m_SoundEngine->play2D(Get(key).c_str(),
			true,
			true,
			true);
		sounds.push_back(s);
		//s->setIsPaused(false);
		return s;
	}

	irrklang::ISound* AudioManager::Init3D(std::string key, glm::vec3 position)
	{
		irrklang::ISound* s = m_SoundEngine->play3D(Get(key).c_str(),
			irrklang::vec3df(position.x, position.y, position.z),
			false,
			true,
			true);
		sounds.push_back(s);
		//s->setIsPaused(false);
		return s;
	}

	irrklang::ISound* AudioManager::InitLooped3D(std::string key, glm::vec3 position)
	{
		irrklang::ISound* s = m_SoundEngine->play3D(Get(key).c_str(),
			irrklang::vec3df(position.x, position.y, position.z),
			true,
			true,
			true);
		//s->setIsPaused(false);
		sounds.push_back(s);
		return s;
	}
	
	std::string AudioManager::AddTrack(std::string path, std::string trackName)
	{
		if (!this->IsResourceInDicktionary(trackName))
		{
	
			this->AddResource(trackName, path);
		}

		return this->Get(trackName);
	}
	void AudioManager::ClearSoundsArray()
	{
		for (auto it = sounds.begin(); it != sounds.end();)
		{
			if ((*it)->isFinished())
			{
				sounds.erase(it);
			}
			else
			{
				++it;
			}
		}
	}
	irrklang::ISoundEngine* AudioManager::GetEngine()
	{
		return m_SoundEngine;
	}
	void AudioManager::SetListenerPosition(glm::vec3 position, glm::vec3 lookDir)
	{
		m_SoundEngine->setListenerPosition(irrklang::vec3df(position.x, position.y, position.z),
			irrklang::vec3df(lookDir.x, lookDir.y, lookDir.z));
	}
}

