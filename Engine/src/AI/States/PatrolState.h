#pragma once

#include "AI/Enemy.h"
#include <AI/State.h>
#include "AI/EnemiesManager.h"
#include <AI/State.h>
#include <AI\NavMesh\NavAgent.h>

namespace sixengine
{
	class PatrolState : public State
	{
	private:

	private:
		glm::vec3 m_OriginalPosition;
		glm::vec3 m_targetPosition;

		SimplePlayer* m_Character;
		EnemiesManager* m_Manager;
		Enemy* m_Enemy;
		NavAgent* m_NavAgent;

	public:
		PatrolState(GameObject* go) : State(go)
		{
			m_Character = Application::Get().GetEntityManager()->EntitiesWithComponents<SimplePlayer>()[0].Component<SimplePlayer>().Get();
			m_Manager = Application::Get().GetEntityManager()->EntitiesWithComponents<EnemiesManager>()[0].Component<EnemiesManager>().Get();
			m_Enemy = go->GetComponent<Enemy>().Get();
			m_NavAgent = go->GetComponent<NavAgent>().Get();

			m_OriginalPosition = m_GameObject->GetComponent<Transform>()->GetWorldPosition();
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
	};
}

