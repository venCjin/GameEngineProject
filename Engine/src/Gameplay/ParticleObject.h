#pragma once

namespace sixengine {

	class ParticleObject
	{
	public:
		ParticleObject() : m_TranslationVelocity(0.0f), m_RotationVelocity(0.0f), m_SizeVelocity(0.0f), m_Active(true), m_Timer(0.0f)
		{}

		ParticleObject(glm::vec3 tVel, glm::vec3 rVel, glm::vec3 sVel) 
			: m_TranslationVelocity(tVel), m_RotationVelocity(rVel), m_SizeVelocity(sVel), m_Active(true), m_Timer(0.0f),
			m_Position(0.0f)
		{}

		~ParticleObject();

		bool m_Active = false;

		glm::vec3 m_TranslationVelocity;
		glm::vec3 m_RotationVelocity;
		glm::vec3 m_SizeVelocity;

		float m_Timer;

		glm::vec3 m_Position;
		//glm::vec3 rotation;
		//glm::vec3 scale;

		glm::mat4 transform = glm::mat4(1.0f);

		// Color - mainly for alpha (effect of disappearing)
		// Texture

		void UpdateTransform(float dt);
		glm::mat4 GetTransform();
	};
}

