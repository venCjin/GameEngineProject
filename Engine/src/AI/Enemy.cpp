#include "pch.h"
#include "Enemy.h"

#include <Gameplay\Components\SimplePlayer.h>
#include <Physics/Components/DynamicBody.h>
#include <glm/gtx/vector_angle.hpp>

#include <AI/StateMachine.h>
#include <AI/States/AttackState.h>
#include <AI/States/IdleState.h>
#include <AI/States/SearchState.h>
#include <AI/States/PatrolState.h>
#include <AI/States/DeathState.h>

sixengine::Enemy::Enemy(GameObject* go)
{
	m_GameObject = go;
	m_Transform = go->GetComponent<Transform>().Get();

	m_SimplePlayer = GetPlayer().Component<SimplePlayer>().Get();
	m_Manager = Application::Get().GetEntityManager()->EntitiesWithComponents<EnemiesManager>()[0].Component<EnemiesManager>().Get();

	m_LastKnownChatacterPosition = glm::vec3(NAN, NAN, NAN);

	m_OriginalPosition = m_Transform->GetWorldPosition();
}

bool sixengine::Enemy::CanSeePlayer()
{
	if (m_SimplePlayer->m_OnSurface == false)
	{
		return false;
	}

	auto player = GetPlayer();

	auto forward = m_GameObject->GetComponent<Transform>()->GetForward();
	auto v = player.Component<Transform>()->GetWorldPosition() - m_GameObject->GetComponent<Transform>()->GetWorldPosition();

	if (glm::dot(forward, forward) > 0) { forward = glm::normalize(forward); }
	if (glm::dot(v, v) > 0) { v = glm::normalize(v); }

	return (glm::dot(forward, v) > glm::cos(glm::radians(45.0f)));
}

bool sixengine::Enemy::CanSeeUndergroundMovement()
{
	if (CanSeePlayer())
	{
		m_LastCharacterSeenTime = Timer::Instance()->ElapsedTime();
		m_LastKnowUndergroundPosition = GetPlayer().Component<Transform>()->GetWorldPosition();
	}

	if (GetPlayer().Component<SimplePlayer>()->m_OnSurface == true)
	{
		return false;
	}

	glm::vec3 a = GetPlayer().Component<Transform>()->GetWorldPosition(); a.y = 0.0f;
	glm::vec3 b = m_GameObject->GetComponent<Transform>()->GetWorldPosition(); b.y = 0.0f;

	if (glm::length(a - b) > 3.0f)
	{
		return false;
	}

	if (glm::length(GetPlayer().Component<DynamicBody>()->m_Velocity) < 0.1f)
	{
		if (m_LastCharacterSeenTime + 0.6f > Timer::Instance()->ElapsedTime())
		{
			return true;
		}

		return false;
	}

	glm::vec3 direction = GetPlayer().Component<Transform>()->GetWorldPosition() - m_GameObject->GetComponent<Transform>()->GetWorldPosition();
	direction.y = 0.0f;

	if (glm::angle(direction, m_GameObject->GetComponent<Transform>()->GetForward()) > 90.0f)
	{
		if (m_GameObject->GetComponent<StateMachine>()->IsCurrentlyInState(typeid(AttackState*)))
		{
			return false;
		}
	}

	// Jesli nic nie stoi na preskodie
	if (true)
	{
		m_LastKnowUndergroundPosition = m_GameObject->GetComponent<Transform>()->GetWorldPosition();
		m_LastUndergroundMovementSeenTime = Timer::Instance()->ElapsedTime();
	
		return true;
	}

	if (m_LastUndergroundMovementSeenTime + 0.5f > Timer::Instance()->ElapsedTime())
	{
		return true;
	}

	return false;
}

bool sixengine::Enemy::HasDetectedPlayer()
{
	if (Timer::Instance()->ElapsedTime() - m_LastDetectionUpdateTime >= 0.5f * Timer::Instance()->DeltaTime())
	{
		if (m_GameObject->GetComponent<StateMachine>()->IsCurrentlyInState(typeid(IdleState*)) || m_GameObject->GetComponent<StateMachine>()->IsCurrentlyInState(typeid(PatrolState*)))
		{
			if (CanSeePlayer() == true)
			{
				float distance = glm::length(m_Transform->GetWorldPosition() - GetPlayer().Component<Transform>()->GetWorldPosition());
				float speed = 0.4f;

				m_DetectionLevel += speed * Timer::Instance()->DeltaTime();
			}
			else if (Timer::Instance()->DeltaTime() > m_LastCharacterSeenTime + 1.0f)
			{
				float speed = 0.1f * m_Parameters.TimeToForget;
				m_DetectionLevel -= speed * Timer::Instance()->DeltaTime();
			}
		}
		else if (m_GameObject->GetComponent<StateMachine>()->IsCurrentlyInState(typeid(SearchState*)))
		{
			m_DetectionLevel = 1.0f;
		}
		else if (m_GameObject->GetComponent<StateMachine>()->IsCurrentlyInState(typeid(AttackState*)))
		{
			m_DetectionLevel = 1.0f;
		}

		m_DetectionLevel = std::clamp(m_DetectionLevel, 0.0f, 1.0f);

		m_LastDetectionUpdateTime = Timer::Instance()->ElapsedTime();
	}

	return (CanSeePlayer() && m_DetectionLevel >= 1.0f);
}

bool sixengine::Enemy::ChatacterPositionExists()
{
	return (isnan(glm::length(m_LastKnownChatacterPosition)) == false);
}

void sixengine::Enemy::ClearCharacterPosition()
{
	m_LastKnownChatacterPosition = glm::vec3(NAN, NAN, NAN);
}

void sixengine::Enemy::SetCharacterPosition(glm::vec3 position)
{
	m_LastKnownChatacterPosition = position;
}

glm::vec3 sixengine::Enemy::GetCharacterPosition()
{
	return m_LastKnownChatacterPosition;
}

bool sixengine::Enemy::UndergroundPositionExists()
{
	return (isnan(glm::length(m_LastKnowUndergroundPosition)) == false);
}

void sixengine::Enemy::ClearUndergroundPosition()
{
	m_LastKnowUndergroundPosition = glm::vec3(NAN, NAN, NAN);
}

void sixengine::Enemy::SetUndergroundPosition(glm::vec3 position)
{
	m_LastKnowUndergroundPosition = position;
}

glm::vec3 sixengine::Enemy::GetUndergroundPosition()
{
	return m_LastKnowUndergroundPosition;
}

glm::vec3 sixengine::Enemy::GetOriginalPosition()
{
	return m_OriginalPosition;
}

float sixengine::Enemy::GetDetectionLevel()
{
	return m_DetectionLevel;
}

void sixengine::Enemy::ReceiveDamage(float damage)
{
	m_Health -= damage;

	if (m_Health <= 0)
	{
		m_GameObject->GetComponent<Transform>()->SetWorldPosition(0.0f, -100.0f, 0.0f);

		m_GameObject->AddComponent<DeathState>(m_GameObject);
		m_GameObject->GetComponent<StateMachine>()->ChangeState(m_GameObject->GetComponent<DeathState>().Get());

		m_GameObject->GetComponent<StateMachine>()->m_States.clear();
	}
}

void sixengine::Enemy::RotateTowardsVelocity()
{
	auto direction = m_GameObject->GetComponent<DynamicBody>()->m_Velocity;
	direction.y = 0.0f;

	if (glm::length(direction) < 0.1f)
	{
		return;
	}

	auto target = glm::quatLookAt(direction, glm::vec3(0.0f, 1.0f, 0.0f));
	target = glm::normalize(target);

	auto current = m_GameObject->GetComponent<Transform>()->GetWorldRotation();

	auto final = glm::slerp(current, target, 2.5f * (float)Timer::Instance()->DeltaTime());

	m_GameObject->GetComponent<Transform>()->SetWorldRotation(final);
}

Entity sixengine::Enemy::GetPlayer()
{
	return Application::Get().GetEntityManager()->EntitiesWithComponents<SimplePlayer>()[0];
}

void sixengine::Enemy::UpdateDetectionLevel()
{

}
