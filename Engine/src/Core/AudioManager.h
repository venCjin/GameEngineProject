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
		irrklang::ISound* Init(std::string key);
		irrklang::ISound* InitLooped(std::string key);
		irrklang::ISound* Init3D(std::string key, glm::vec3 position);
		irrklang::ISound* InitLooped3D(std::string key, glm::vec3 position);
		std::string AddTrack(std::string path, std::string trackName);
		void ClearSoundsArray();
		irrklang::ISoundEngine* GetEngine();
		void SetListenerPosition(glm::vec3 position, glm::vec3 lookDir);
		//use PLAY macro to play tracks

	};


}
#define INIT_TRACK(...) AudioManager::getInstance()->Init(__VA_ARGS__)

#define ADD_TRACK(...) AudioManager::getInstance()->AddTrack(__VA_ARGS__)

#define INIT_TRACK_3D(...)  AudioManager::getInstance()->Init3D(__VA_ARGS__)

#define INIT_TRACK_3D_LOOPED(...)  AudioManager::getInstance()->InitLooped3D(__VA_ARGS__)