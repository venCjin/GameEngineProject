#pragma once

#include "Engine.h"
#include "AI/EnemiesManager.h"

namespace sixengine
{
	class Enemy
	{
	private:
		class Parameters
		{
		public:
			float angle = 45.0f;
			float angularAcceleration = 10.0f;

			float TimeToForget = 2.0f;

			float UndergroundMovementDistance = 3.0f;
		};

		Parameters m_Parameters;
		
		GameObject* m_GameObject = NULL;
		Transform* m_Transform = NULL;

		class SimplePlayer* m_SimplePlayer = NULL;
		class EnemiesManager* m_Manager = NULL;


		float m_LastCharacterSeenTime = 0.0f;
		float m_LastUndergroundMovementSeenTime = -10.0f;

		glm::vec3 m_LastKnownChatacterPosition;
		glm::vec3 m_LastKnowUndergroundPosition;
		glm::vec3 m_OriginalPosition;
		
		float m_DetectionLevel = 0.0f;

		double m_LastDetectionUpdateTime = 0.0f;;

	public:
		float m_Health = 100.0f;

		Enemy(GameObject* go);
		
		Entity GetPlayer();

		void UpdateDetectionLevel();

		bool CanSeePlayer();
		bool CanSeeUndergroundMovement();
		bool HasDetectedPlayer();

		bool ChatacterPositionExists();
		void ClearCharacterPosition();
		void SetCharacterPosition(glm::vec3 position);
		glm::vec3 GetCharacterPosition();
		
		bool UndergroundPositionExists();
		void ClearUndergroundPosition();
		void SetUndergroundPosition(glm::vec3 position);
		glm::vec3 GetUndergroundPosition();

		glm::vec3 GetOriginalPosition();
		float GetDetectionLevel();

		void ReceiveDamage(float damage);
		void RotateTowardsVelocity();

		//float GetDetectionLevel();
	};
}

