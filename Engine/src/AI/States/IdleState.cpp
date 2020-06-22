#include "pch.h"
#include "IdleState.h"

#include <AI/Enemy.h>

bool sixengine::IdleState::IsStateReady(StateMachine* stateMachine)
{
	if (m_Enemy->ChatacterPositionExists() == false && m_Enemy->UndergroundPositionExists() == false)
	{
		m_GameObject->GetComponent<StateMachine>()->ChangeState(this);
	}

	return true;
}

void sixengine::IdleState::OnStateEnter(StateMachine* stateMachine, State* previousState)
{
	m_GameObject->GetComponent<Enemy>()->ClearCharacterPosition();
	m_GameObject->GetComponent<Enemy>()->ClearUndergroundPosition();

	m_GameObject->GetComponent<Enemy>()->m_DetectionLevel = 0.0f;

	m_GameObject->GetComponent<Animation>()->ChangeAnimation("walk");
	m_NacAgent->SetDestination(m_OriginalPosition);
	m_NacAgent->Go();
}

void sixengine::IdleState::OnStateUpdate(StateMachine* stateMachine)
{
	LOG_INFO("IdleState reporting on duty");
	
	if (m_NacAgent->GetRemainingDistance() < 0.25f)
	{
		m_GameObject->GetComponent<Animation>()->ChangeAnimation("idle");
		m_GameObject->GetComponent<Transform>()->SetWorldPosition(m_OriginalPosition);
		//m_GameObject->GetComponent<Transform>()->SetWorldRotation(-m_OriginalRotation);
		
		m_NacAgent->Stop();
	}

	m_Enemy->RotateTowardsVelocity();
}

bool sixengine::IdleState::IsStateFinished()
{
	return true;
}

void sixengine::IdleState::OnStateExit()
{
	
}
