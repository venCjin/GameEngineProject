#pragma once

#include <Engine.h>
#include <Gameplay/Components/SimplePlayer.h>

namespace sixengine
{
	class EnemiesManager
	{
	private:
		Entity player;
		glm::vec3 m_LastKnownChatacterPosition;

	public:
		EnemiesManager();
		
		bool HasAnybodyDetectedPlayer();
		
		glm::vec3 GetCharacterPosition();
		bool LastCharacterPositionExists();
	};
}

