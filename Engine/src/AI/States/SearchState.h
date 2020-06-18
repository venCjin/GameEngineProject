#pragma once

#include "AI/Enemy.h"
#include <AI/State.h>
#include "AI/EnemiesManager.h"
#include <AI/State.h>
#include <AI/NavMesh/NavAgent.h>

namespace sixengine
{
	class SearchState : public State
	{
	private:
		class Parameters
		{
		public:
			float duration = 7.5f;
			float minDistance = 3.0f;
			float maxDistance = 6.0f;
		};

		Parameters m_Parameters;
		glm::vec3 m_TargetPosition;
		float m_BehaviourTime = 0.0f;
		float m_WaitingTime = 0.0f;

		SimplePlayer* m_Character;
		EnemiesManager* m_Manager;
		Enemy* m_Enemy;
		NavAgent* m_NavAgent;

	public:
		SearchState(GameObject* go) : State(go)
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

		void GoToRandomPlace(float min, float max);
	};
}

