#pragma once

#include "Engine.h"
#include "StateMachine.h"

namespace sixengine
{
	class State
	{
	protected:
		GameObject* m_GameObject;

	public:
		State(GameObject* go)
		{
			m_GameObject = go;
		}

		virtual const type_info& GetType() = 0;

		virtual bool IsStateReady(class StateMachine* stateMachine) { return false; }
		virtual void OnStateEnter(class StateMachine* stateMachine, State* previousState) { }
		virtual void OnStateUpdate(class StateMachine* stateMachine) { };
		virtual bool IsStateFinished() { return true; }
		virtual void OnStateExit() { }
	};
}