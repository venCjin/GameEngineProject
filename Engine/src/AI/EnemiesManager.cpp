#include "pch.h"
#include "EnemiesManager.h"

#include "Enemy.h"
#include "AI/StateMachine.h"
#include "AI/States/AttackState.h"

sixengine::EnemiesManager::EnemiesManager()
{
	player = Application::Get().GetEntityManager()->EntitiesWithComponents<SimplePlayer>()[0];
}

bool sixengine::EnemiesManager::HasAnybodyDetectedPlayer()
{
	//for(auto enemy : Application::Get().GetEntityManager()->EntitiesWithComponents<Enemy>())
	//{
	//	if (enemy.Component<Enemy>()->HasDetectedPlayer())
	//	{
	//		m_LastKnownChatacterPosition = player.Component<Transform>()->GetWorldPosition();
	//		return true;
	//	}
	//}

	for (auto enemy : Application::Get().GetEntityManager()->EntitiesWithComponents<Enemy>())
	{
		if (enemy.Component<StateMachine>()->IsCurrentlyInState(typeid(AttackState*)))
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
