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
		std::vector<irrklang::ISound*> sounds;
		static AudioManager* getInstance();
		void Play(std::string key);
		void PlayLooped(std::string key);
		void Play3D(std::string key, glm::vec3 position);
		void PlayLooped3D(std::string key, glm::vec3 position);
		std::string AddTrack(std::string path, std::string trackName);
		void ClearSoundsArray();
		irrklang::ISoundEngine* GetEngine();
		void SetListenerPosition(glm::vec3 position, glm::vec3 lookDir);
		//use PLAY macro to play tracks

	};


}
#define PLAY_TRACK(...) AudioManager::getInstance()->Play(__VA_ARGS__)

#define ADD_TRACK(...) AudioManager::getInstance()->AddTrack(__VA_ARGS__)

#define PLAY_TRACK_3D(...)  AudioManager::getInstance()->Play3D(__VA_ARGS__)

#define PLAY_TRACK_3D_LOOPED(...)  AudioManager::getInstance()->PlayLooped3D(__VA_ARGS__)