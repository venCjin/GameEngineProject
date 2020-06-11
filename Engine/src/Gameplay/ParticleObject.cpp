#pragma once

#include "pch.h"
#include "ParticleObject.h"
#include <ECS/EntityManager.h>

namespace sixengine {

	ParticleObject::~ParticleObject()
	{
	}

	void ParticleObject::UpdateTransform(float dt, const glm::mat4& viewMatrix)
	{
		//m_Position += m_TranslationVelocity * dt;
		m_Transform.Translate(m_TranslationVelocity * dt);

		glm::mat4 matrix(1.0f);
		matrix = glm::translate(matrix, m_Transform.getWorldPosition());
		matrix[0] = glm::vec4(viewMatrix[0][0], viewMatrix[1][0], viewMatrix[2][0], 0.0f);
		matrix[1] = glm::vec4(viewMatrix[0][1], viewMatrix[1][1], viewMatrix[2][1], 0.0f);
		matrix[2] = glm::vec4(viewMatrix[0][2], viewMatrix[1][2], viewMatrix[2][2], 0.0f);

		m_Transform.SetWorld(matrix);

		// transform.position += m_TranslationVelocity * dt
	}

	glm::mat4 ParticleObject::GetTransform()
	{
		/*glm::mat4 translation = glm::mat4(1.0f);
		translation = glm::translate(translation, m_Position);

		glm::mat4 rotation = glm::mat4(1.0f);
		glm::mat4 scale = glm::mat4(1.0f);*/

		//return translation * rotation * scale;

		return m_Transform.GetModelMatrix();
	}

}