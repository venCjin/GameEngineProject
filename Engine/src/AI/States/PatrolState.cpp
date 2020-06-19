#include "pch.h"
#include "PatrolState.h"

bool sixengine::PatrolState::IsStateReady(StateMachine* stateMachine)
{
	return true;
}

void sixengine::PatrolState::OnStateEnter(StateMachine* stateMachine, State* previousState)
{
	m_GameObject->GetComponent<Enemy>()->ClearCharacterPosition();
	m_GameObject->GetComponent<Enemy>()->ClearUndergroundPosition();

	m_targetPosition = m_OriginalPosition + glm::vec3(0.0f, 0.0f, 0.0f);
	//m_targetPosition = m_OriginalPosition + glm::vec3(rand, 0.0f, rand);

	//_enemy.navMeshAgent.destination = _targetPosition;
	//_enemy.navMeshAgent.isStopped = false;
}

void sixengine::PatrolState::OnStateUpdate(StateMachine* stateMachine)
{
	//if (_enemy.navMeshAgent.remainingDistance < 1.5f)
	//{
	//	m_targetPosition = m_OriginalPosition + glm::vec3(0.0f, 0.0f, 0.0f);
	//	//m_targetPosition = m_OriginalPosition + glm::vec3(rand, 0.0f, rand);

	//	//_enemy.navMeshAgent.destination = _targetPosition;
	//	//_enemy.navMeshAgent.isStopped = false;
	//}
}

bool sixengine::PatrolState::IsStateFinished()
{
	return true;
}

void sixengine::PatrolState::OnStateExit()
{

}
