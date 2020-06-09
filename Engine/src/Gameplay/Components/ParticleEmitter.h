#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Core/ISerializable.h"
#include "Gameplay/ParticleObject.h"

#include "Renderer/Texture.h"

#define MAX_PARTICLE_COUNT 500

namespace sixengine {

	class ParticleEmitter// : public ISerializable
	{
	private:
		int m_LastUnusedParticle = 0;

	public:
		float m_EmissionDuration = 2.0f;
		float m_EmissionFrequency = 10.0f; // in Hertz, so number of particles per second
		
		float m_ParticleLifeDuration = 5.0f;
		bool m_Loop = true;
		
		float m_StartSpeed = 0.5f; 
		// m_StartSize?

		float m_TranslationAcceleration = 0.0f;
		float m_RotationAcceleration = 0.0f;
		float m_SizeAcceleration = 0.0f;
		ParticleObject m_Particles[MAX_PARTICLE_COUNT];

		Texture* m_Texture;

		ParticleEmitter() { }

		ParticleEmitter(Texture* texture)
			: m_Texture(texture)
		{
		}
	
		int FindUnusedParticle()
		{
			for (int i = m_LastUnusedParticle; i < MAX_PARTICLE_COUNT; i++)
			{
				if (!m_Particles[i].m_Active)
				{
					m_LastUnusedParticle = i;
					return m_LastUnusedParticle;
				}
			}

			for (int i = 0; i < m_LastUnusedParticle; i++)
			{
				if (!m_Particles[i].m_Active)
				{
					m_LastUnusedParticle = i;
					return m_LastUnusedParticle;
				}
			}

			return 0;
		}

		void SpawnParticle()
		{
			glm::vec3 direction = glm::vec3(0.0f, 1.0f, 0.0f);

			glm::vec3 startTranslationVelocity = direction * m_StartSpeed;
			glm::vec3 startRotationVelocity = glm::vec3(0.0f);
			glm::vec3 startScaleVelocity = glm::vec3(0.0f);

			m_Particles[FindUnusedParticle()] = ParticleObject(startTranslationVelocity, startRotationVelocity, startScaleVelocity);
		}

		std::vector<glm::mat4> GetAllTransforms()
		{
			std::vector<glm::mat4> transforms;
			transforms.reserve(MAX_PARTICLE_COUNT);

			for (int i = 0; i < MAX_PARTICLE_COUNT; i++)
			{
				if (m_Particles[i].m_Active)
					transforms.push_back(m_Particles[i].GetTransform());
			}

			return transforms;
		}

		

	};
}
	