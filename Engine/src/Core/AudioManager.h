#pragma once
#include <Engine.h>
#include <Core/ResourceManager.h>
#include <irrKlang.h>

namespace sixengine
{
	class AudioManager : public ResourceManager<std::string>
	{
		irrklang::ISoundEngine* m_SoundEngine;
		static AudioManager* instance;
		AudioManager();
	public:
		static AudioManager* getInstance();
		void Play(std::string key);
		std::string AddTrack(std::string path, std::string trackName);

		//use PLAY macro to play tracks

	};


}
#define PLAY_TRACK(...) AudioManager::getInstance()->Play(__VA_ARGS__)
#define ADD_TRACK(...) AudioManager::getInstance()->AddTrack(__VA_ARGS__)