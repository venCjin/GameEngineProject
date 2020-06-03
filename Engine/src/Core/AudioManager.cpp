#include "pch.h"
#include "AudioManager.h"

namespace sixengine
{

	AudioManager* AudioManager::instance = nullptr;
	AudioManager::AudioManager() : ResourceManager<std::string>()
	{
		this->m_SoundEngine = irrklang::createIrrKlangDevice();
	}

	AudioManager* AudioManager::getInstance()
	{
		if (instance == nullptr)
		{
			instance = new AudioManager();
		}
		return instance;
	}

	void AudioManager::Play(std::string key)
	{
		m_SoundEngine->play2D(Get(key).c_str());
	}
	
	std::string AudioManager::AddTrack(std::string path, std::string trackName)
	{
		if (!this->IsResourceInDicktionary(trackName))
		{
	
			this->AddResource(trackName, path);
		}

		return this->Get(trackName);
	}
}

