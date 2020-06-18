#include "pch.h"
#include "StateMachine.h"

sixengine::StateMachine::StateMachine()
{

}

void sixengine::StateMachine::OnUpdate()
{
	for(auto state : m_States)
	{
		if (state == m_CurrentState) { continue; }

		if (state->IsStateReady(this) && m_CurrentState->IsStateFinished())
		{
			this->ChangeState(state);
			break;
		}
	}

	if (m_CurrentState != NULL)
	{
		m_CurrentState->OnStateUpdate(this);
	}
}

void sixengine::StateMachine::ChangeState(State* newState)
{
	auto previousState = m_CurrentState;

	if (m_CurrentState != NULL)
	{
		m_CurrentState->OnStateExit();
	}

	m_CurrentState = newState;

	m_CurrentState->OnStateEnter(this, previousState);
}

bool sixengine::StateMachine::IsCurrentlyInState(const type_info& state)
{
	return m_CurrentState->GetType() == state;
}

const type_info& sixengine::StateMachine::GetCurrentStateType()
{
	return (typeid(m_CurrentState));
}
