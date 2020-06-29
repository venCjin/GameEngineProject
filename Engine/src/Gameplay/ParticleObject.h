#pragma once

#include <Gameplay/Components/Transform.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Core/CameraSystem/Camera.h"

namespace sixengine {

	struct ParticleData
	{
		glm::mat4 transformMatrix;
		glm::vec4 color;
	};

	class ParticleObject
	{
	public:		


		ParticleObject() : m_TranslationVelocity(glm::vec3(0.0f)), m_Active(true), m_Timer(0.0f)
		{
			//m_Transform.SetLocalPosition(glm::vec3(0.0f, 0.0f, 0.0f));
			m_Transform.SetLocalScale(glm::vec3(0.0f, 0.0f, 0.0f));
			m_Transform.SetLocalOrientation(glm::vec3(0.0f, 90.0f, 0.0f));
		}

		ParticleObject(glm::vec3 parentPos, glm::vec3 translationVel, float startSize, float sizeSpeed, bool camAlign) 
			: m_TranslationVelocity(translationVel), m_StartSize(startSize), m_SizeSpeed(sizeSpeed), m_CameraAlignment(camAlign), m_Active(true), m_Timer(0.0f)
		{
			m_Transform.SetLocalPosition(parentPos);
			m_Transform.SetLocalOrientation(glm::vec3(0.0f, 90.0f, 0.0f));
			m_Transform.SetLocalScale(glm::vec3(m_StartSize, m_StartSize, m_StartSize));
		}

		~ParticleObject();

		bool m_Active = false;

		glm::vec3 m_TranslationVelocity;
		float m_StartSize;
		float m_SizeSpeed;

		float m_Timer;
		bool m_CameraAlignment;

		glm::vec4 m_Color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

		Transform m_Transform;

		void UpdateTransform(float dt, const glm::mat4& viewMatrix);
		ParticleData GetParticleData();
	};
}

