#pragma once

#include "AI/State.h"

namespace sixengine
{
	class StateMachine
	{
	public:
		class State* m_CurrentState;
		class std::vector<State*> m_States;

	public:
		StateMachine();

		virtual void OnUpdate();
		
		void ChangeState(State* newState);
		bool IsCurrentlyInState(const type_info& state);
		const type_info& GetCurrentStateType();
	};
}