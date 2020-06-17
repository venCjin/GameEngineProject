#include "pch.h"
#include "EnemiesManager.h"

#include "Enemy.h"

sixengine::EnemiesManager::EnemiesManager()
{
	player = Application::Get().GetEntityManager()->EntitiesWithComponents<SimplePlayer>()[0];
}

bool sixengine::EnemiesManager::HasAnybodyDetectedPlayer()
{
	for(auto enemy : Application::Get().GetEntityManager()->EntitiesWithComponents<Enemy>())
	{
		if (enemy.Component<Enemy>()->HasDetectedPlayer())
		{
			m_LastKnownChatacterPosition = player.Component<Transform>()->GetWorldPosition();
			return true;
		}
	}

	return false;
}

glm::vec3 sixengine::EnemiesManager::GetCharacterPosition()
{
	return m_LastKnownChatacterPosition;
}

bool sixengine::EnemiesManager::LastCharacterPositionExists()
{
	return (isnan(glm::length(m_LastKnownChatacterPosition)) == false);
}
