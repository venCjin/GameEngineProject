#pragma once
#include <ECS/SystemManager.h>
#include <Gameplay/Components/AirText.h>
#include <Gameplay\Components\SimplePlayer.h>
#include <Gameplay\Components\Text.h>
#include <Gameplay\Components\Eggs.h>

#include <Physics/Systems/CollisionSystem.h>


namespace sixengine
{
	SYSTEM(WinLossSystem, SimplePlayer)
	{
	public:
		GameObject* winLoseText;

	private:
		void OnCollisionHandle(BaseEvent & e)
		{
			OnCollision& collisionEvent = dynamic_cast<OnCollision&>(e);

			if (collisionEvent.m_Entity.HasComponent<SimplePlayer>() &&
				collisionEvent.collision.other.HasComponent<Eggs>())
			{
				winLoseText->GetComponent<Text>()->SetText("Congratulations!");
				winLoseText->GetComponent<Transform>()->SetWorldPosition(260.0, 340.0f, 0.0f);
				Timer::Instance()->SetPaused(true);
			}
		}

	public:

		void OnStart(EventManager& eventManager) override
		{
			eventManager.AddListener<OnCollision>(&WinLossSystem::OnCollisionHandle, this);
		}

		void Update(EventManager & eventManager, float dt) override
		{
			if (m_SimplePlayer->m_Health <= 0)
			{
				winLoseText->GetComponent<Text>()->SetText("You died!");
				winLoseText->GetComponent<Transform>()->SetWorldPosition(400.0, 340.0f, 0.0f);
				Timer::Instance()->SetPaused(true);
			} 
			//else
				//m_SimplePlayer->m_Health -= 0.5f;
		}
	};
}