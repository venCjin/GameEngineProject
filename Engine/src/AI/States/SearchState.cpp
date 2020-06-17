#include "pch.h"
#include "SearchState.h"

#include <AI/States/AttackState.h>
#include <random>

bool sixengine::SearchState::IsStateReady(StateMachine* stateMachine)
{
	glm::vec3 a = m_Character->gameObject->GetComponent<Transform>()->GetWorldPosition();
	glm::vec3 b = m_GameObject->GetComponent<Transform>()->GetWorldPosition();

	float distanceToPlayer = glm::length(a - b);

	if (m_Manager->HasAnybodyDetectedPlayer() && distanceToPlayer > 10.0f && distanceToPlayer < 25.0f)
	{
		return true;
	}

	LOG_INFO(m_Manager->HasAnybodyDetectedPlayer());

	return (m_GameObject->GetComponent<StateMachine>()->IsCurrentlyInState(typeid(AttackState*)) && m_Manager->HasAnybodyDetectedPlayer() == false);

}

void sixengine::SearchState::OnStateEnter(StateMachine* stateMachine, State* previousState)
{
	m_BehaviourTime = 0.0f;
	GoToRandomPlace(0.0f, 1.75f);
}

void sixengine::SearchState::OnStateUpdate(StateMachine* stateMachine)
{
	LOG_INFO("SearchState reporting on duty");

	if (m_NavAgent->GetRemainingDistance() < 1.0f)
	{
		m_NavAgent->m_ProcedeMoving = false;

		if (m_WaitingTime > 1.25f)
		{
			m_WaitingTime = 0.0f;
			GoToRandomPlace(m_Parameters.minDistance, m_Parameters.maxDistance);
		}
		else
		{
			m_WaitingTime += (float)Timer::Instance()->DeltaTime();
		}
	}

	m_BehaviourTime += (float)Timer::Instance()->DeltaTime();

	m_Enemy->RotateTowardsVelocity();
}

bool sixengine::SearchState::IsStateFinished()
{
	glm::vec3 a = m_Character->gameObject->GetComponent<Transform>()->GetWorldPosition();
	glm::vec3 b = m_GameObject->GetComponent<Transform>()->GetWorldPosition();

	float distanceToPlayer = glm::length(a - b);
	return ((m_Manager->HasAnybodyDetectedPlayer() && distanceToPlayer < 15.0f) || m_BehaviourTime > m_Parameters.duration);

}

void sixengine::SearchState::OnStateExit()
{
	m_BehaviourTime = 0.0f;
}

void sixengine::SearchState::GoToRandomPlace(float min, float max)
{
	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_real_distribution<double> radiusGenerator(-1.0f, 1.0f);
	std::uniform_real_distribution<double> distanceGenerator(min, max);

	glm::vec3 direction = glm::vec3(radiusGenerator(mt), 0.0f, radiusGenerator(mt));
	glm::normalize(direction);

	float distance = distanceGenerator(mt);

	if (m_Enemy->UndergroundPositionExists())
	{
		m_TargetPosition = m_Enemy->GetUndergroundPosition() + direction * distance;
	}
	else
	{
		m_TargetPosition = m_Enemy->GetOriginalPosition() + direction * distance;
	}

	m_NavAgent->SetDestination(m_TargetPosition);
	m_NavAgent->Go();
}
