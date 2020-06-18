#include "pch.h"
#include "DeathState.h"

bool sixengine::DeathState::IsStateReady(StateMachine* stateMachine)
{
	return false;
}

void sixengine::DeathState::OnStateEnter(StateMachine* stateMachine, State* previousState)
{
}

void sixengine::DeathState::OnStateUpdate(StateMachine* stateMachine)
{
}

bool sixengine::DeathState::IsStateFinished()
{
	return false;
}

void sixengine::DeathState::OnStateExit()
{
}
