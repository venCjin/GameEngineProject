#include "pch.h"

#include <ECS/EntityManager.h>

#include <AI/States/SearchState.h>

#include "AttackState.h"
#include "Gameplay/Components/SimplePlayer.h"

#include <glm/gtx/vector_angle.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/quaternion.hpp>

#include <Physics/Components/DynamicBody.h>

#include <Renderer/Gizmo.h>

bool sixengine::AttackState::IsStateReady(StateMachine* stateMachine)
{
	if (m_Enemy->CanSeeUndergroundMovement())
	{
		m_GameObject->GetComponent<StateMachine>()->ChangeState(this);
	}

	glm::vec3 a = m_Character->gameObject->GetComponent<Transform>()->GetWorldPosition();
	glm::vec3 b = m_GameObject->GetComponent<Transform>()->GetWorldPosition();

	float distanceToPlayer = glm::length(a - b);

	if (m_Manager->HasAnybodyDetectedPlayer() && (distanceToPlayer < 10.0f))
	{
		m_GameObject->GetComponent<StateMachine>()->ChangeState(this);
		return true;
	}

	return (
		(
			m_GameObject->GetComponent<StateMachine>()->IsCurrentlyInState(typeid(SearchState*))
			&& m_Enemy->CanSeeUndergroundMovement()
			)
		||
		(m_Manager->HasAnybodyDetectedPlayer() && (distanceToPlayer < 10.0f))
		);
}

void sixengine::AttackState::OnStateEnter(StateMachine* stateMachine, State* previousState)
{
	_lastShootTime = Timer::Instance()->ElapsedTime();
}

void sixengine::AttackState::OnStateUpdate(StateMachine* stateMachine)
{
	LOG_INFO("AttackState reporting on duty");

	if (m_Manager->HasAnybodyDetectedPlayer())
	{
		float distanceToDestination = glm::length(m_GameObject->GetComponent<Transform>()->GetWorldPosition() - m_Manager->GetCharacterPosition());

		if (distanceToDestination >= m_Parameters.minShotRange)
		{
			m_NavAgent->SetDestination(m_Manager->GetCharacterPosition());
			m_NavAgent->Go();
		}
		else
		{
			//float t = Mathf.InverseLerp(_parameters.maxShotRange, _parameters.minShotRange, distanceToDestination);
			//float accuracyDistanceFactor = _parameters.accuracyOfDistance.Evaluate(t);
			float accuracyDistanceFactor = 1.0f;

			glm::vec3 forward = m_GameObject->GetComponent<Transform>()->GetForward(); forward.y = 0.0f;
			glm::vec3 offset = m_Character->gameObject->GetComponent<Transform>()->GetWorldPosition() - m_GameObject->GetComponent<Transform>()->GetWorldPosition(); offset.y = 0.0f;
			float angle = glm::degrees(glm::angle(forward, offset));

			//float t = Mathf.InverseLerp(0, _parameters.maxAngle, angle);
			//float accuracyAngleDactor = _parameters.accuracyOfAngle.Evaluate(t);
			float accuracyAngleDactor = 1.0f;

			if (Timer::Instance()->ElapsedTime() > _lastShootTime + m_Parameters.shootCooldown)
			{
				float a = max(accuracyDistanceFactor, accuracyAngleDactor) * min(accuracyDistanceFactor, accuracyAngleDactor);
				Shoot(a);
			}
		}

		if (distanceToDestination <= m_Parameters.minShotRange)
		{
			m_NavAgent->Stop();
		}

		RoateTowardsPlayer();
	}
	else if (m_Enemy->CanSeeUndergroundMovement())
	{
		glm::vec3 a = m_GameObject->GetComponent<Transform>()->GetWorldPosition(); a.y = 0.0f;
		glm::vec3 b = m_Enemy->GetPlayer().Component<Transform>()->GetWorldPosition(); b.y = 0.0f;

		if (glm::length(a - b) >= 0.5f)
		{
			m_NavAgent->SetDestination(b);
			m_NavAgent->Go();
		
			RoateTowardsPlayer();
		}
		else
		{
			m_NavAgent->Stop();
		}
	}
	else
	{
		RoateTowardsPlayer();
	}
}

bool sixengine::AttackState::IsStateFinished()
{
	return (m_Manager->HasAnybodyDetectedPlayer() == false 
		&& m_Enemy->CanSeeUndergroundMovement() == false 
		&& (m_NavAgent->GetRemainingDistance() < 1.5f || m_NavAgent->m_ProcedeMoving == false));
}

void sixengine::AttackState::OnStateExit()
{
	m_NavAgent->Stop();
}

void sixengine::AttackState::RoateTowardsPlayer()
{
	auto player = m_Enemy->GetPlayer();

	auto playerPos = player.Component<Transform>()->GetWorldPosition();
	auto enemyPos = m_GameObject->GetComponent<Transform>()->GetWorldPosition();

	auto direction = glm::normalize(playerPos - enemyPos);
	direction.y = 0.0f;

	auto target = glm::quatLookAt(direction, glm::vec3(0.0f, 1.0f, 0.0f)); 
	target = glm::normalize(target);
	
	auto current = m_GameObject->GetComponent<Transform>()->GetWorldRotation();

	auto final = glm::slerp(current, target, 4.5f * (float)Timer::Instance()->DeltaTime());

	m_GameObject->GetComponent<Transform>()->SetWorldRotation(final);
}

void sixengine::AttackState::Shoot(float accuracy)
{
	//_references.shootParticle.Play();
	_lastShootTime = Timer::Instance()->ElapsedTime();
	
	//StartCoroutine(ShowShootLine());
	glm::vec3 playerPos = m_Enemy->GetPlayer().Component<Transform>()->GetWorldPosition();
	glm::vec3 enemyPos = m_GameObject->GetComponent<Transform>()->GetWorldPosition();
	Gizmo::DrawLine(enemyPos, playerPos, 0.1f);


	//if (UnityEngine.Random.value < accuracy)
	{
		//FindObjectOfType<CharacterController>().ReceiveDamage((int)(accuracy * _parameters.maxDamage));
		//_references.impulseSource.GenerateImpulse();
	}
}
