#pragma once

#include "pch.h"
#include "ParticleObject.h"
#include <ECS/EntityManager.h>

namespace sixengine {

	ParticleObject::~ParticleObject()
	{
	}

	void ParticleObject::UpdateTransform(float dt)
	{
		m_Position += m_TranslationVelocity * dt;
		// transform.position += m_TranslationVelocity * dt
	}

	glm::mat4 ParticleObject::GetTransform()
	{
		glm::mat4 translation = glm::mat4(1.0f);
		translation = glm::translate(translation, m_Position);

		glm::mat4 rotation = glm::mat4(1.0f);
		glm::mat4 scale = glm::mat4(1.0f);

		return translation * rotation * scale;
	}

}