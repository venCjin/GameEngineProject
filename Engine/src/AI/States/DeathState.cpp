#include "pch.h"
#include "DeathState.h"

bool sixengine::DeathState::IsStateReady(StateMachine* stateMachine)
{
	return false;
}

void sixengine::DeathState::OnStateEnter(StateMachine* stateMachine, State* previousState)
{
	m_GameObject->GetComponent<Animation>()->ChangeAnimation("death");
	endOfVisibleTime = Timer::GetTime(SECOND) + endOfVisibleTime;
}

void sixengine::DeathState::OnStateUpdate(StateMachine* stateMachine)
{
	if (Timer::GetTime(SECOND) > endOfVisibleTime)
	{
		m_GameObject->GetComponent<Transform>()->SetWorldPosition(0.0f, -100.0f, 0.0f);
	}
}

bool sixengine::DeathState::IsStateFinished()
{
	return false;
}

void sixengine::DeathState::OnStateExit()
{
}
