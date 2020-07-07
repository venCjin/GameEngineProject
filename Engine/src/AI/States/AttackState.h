#pragma once

#include <Gameplay/Components/SimplePlayer.h>
#include "AI/Enemy.h"
#include "AI/EnemiesManager.h"
#include <AI/State.h>
#include <AI/NavMesh/NavAgent.h>

namespace sixengine
{
	class AttackState : public State
	{
	private:
		class Parameters
		{
		public:
			int maxDamage = 40;
			float maxShotRange = 15.0f;
			float minShotRange = 6.0f;
			float shootCooldown = 0.8f;
			float maxAngle = 7.5f;
		};

		Parameters m_Parameters;
		const float m_Speed = 125.0f;

	private:
		SimplePlayer* m_Character;
		EnemiesManager* m_Manager;
		Enemy* m_Enemy;
		NavAgent* m_NavAgent;

		float _lastShootTime = 0.0f;

		float _originalAngularSpeed;

	public:
		AttackState(GameObject* go) : State(go)
		{
			m_Character = Application::Get().GetEntityManager()->EntitiesWithComponents<SimplePlayer>()[0].Component<SimplePlayer>().Get();
			m_Manager = Application::Get().GetEntityManager()->EntitiesWithComponents<EnemiesManager>()[0].Component<EnemiesManager>().Get();
			m_Enemy = go->GetComponent<Enemy>().Get();
			m_NavAgent = go->GetComponent<NavAgent>().Get();
		}

		virtual const type_info& GetType()
		{
			return typeid(this);
		}

		virtual bool IsStateReady(class StateMachine* stateMachine);
		virtual void OnStateEnter(class StateMachine* stateMachine, State* previousState);
		virtual void OnStateUpdate(class StateMachine* stateMachine);
		virtual bool IsStateFinished();
		virtual void OnStateExit();

		void RoateTowardsPlayer();
		void Shoot(float accuracy);
	};
}

