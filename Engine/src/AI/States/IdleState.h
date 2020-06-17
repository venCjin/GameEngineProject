#pragma once

#include <AI/State.h>
#include <AI/NavMesh/NavAgent.h>

namespace sixengine
{
	class IdleState : public State
	{
	private:
		class Enemy* m_Enemy = NULL;
		class NavAgent* m_NacAgent = NULL;

		glm::vec3 m_OriginalPosition;
		glm::quat m_OriginalRotation;

	public:
		IdleState(GameObject* go) : State(go)
		{
			m_Enemy = go->GetComponent<Enemy>().Get();
			m_NacAgent = go->GetComponent<NavAgent>().Get();

			m_OriginalPosition = go->GetComponent<Transform>()->GetWorldPosition();
			m_OriginalRotation = go->GetComponent<Transform>()->GetWorldRotation();
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

