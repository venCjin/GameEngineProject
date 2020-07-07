#pragma once

#include <AI/State.h>

namespace sixengine
{
	class DeathState : public State
	{
	public:
		float endOfVisibleTime = 3.5f;
		DeathState(GameObject* go) : State(go)
		{
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

