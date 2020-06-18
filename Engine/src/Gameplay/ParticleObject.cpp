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
		m_Timer += dt;

		m_Transform.Translate(m_TranslationVelocity * dt);

		if (m_CameraAlignment)
			m_Transform.LookAt(Camera::ActiveCamera->m_Transform);

		m_Transform.SetLocalScale(m_Transform.GetLocalScale() + (glm::vec3(1.0f, 1.0f, 1.0f) * m_SizeSpeed) * dt);

		/*glm::mat4 matrix(1.0f);
		matrix = glm::translate(matrix, m_Transform.getWorldPosition());
		matrix[0] = glm::vec4(viewMatrix[0][0], viewMatrix[1][0], viewMatrix[2][0], 0.0f);
		matrix[1] = glm::vec4(viewMatrix[0][1], viewMatrix[1][1], viewMatrix[2][1], 0.0f);
		matrix[2] = glm::vec4(viewMatrix[0][2], viewMatrix[1][2], viewMatrix[2][2], 0.0f);
		matrix = glm::scale(matrix, glm::vec3(5.0f, 5.0f, 5.0f));

		m_Transform.SetWorld(matrix);
		*/

	}

	

	ParticleData ParticleObject::GetParticleData()
	{
		ParticleData pd;
		pd.transformMatrix = m_Transform.GetModelMatrix();
		pd.color = m_Color;

		return pd;
	}

}