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
		float m_EmissionDuration;
		float m_EmissionFrequency; // in Hertz, so number of particles per second
		
		float m_ParticleLifeDuration;
		bool m_Loop;
		
		float m_StartSpeed; 
		// m_StartSize?

		float m_TranslationAcceleration;
		float m_RotationAcceleration;
		float m_SizeAcceleration;
		//ParticleObject m_Particles[MAX_PARTICLE_COUNT];
		std::vector<ParticleObject*> m_ParticlesVector;

		Texture* m_Texture;

		ParticleEmitter() 
		{
			//m_ParticlesVector.resize(MAX_PARTICLE_COUNT);
			for (size_t i = 0; i < MAX_PARTICLE_COUNT; i++)
			{
				ParticleObject *po = new ParticleObject();
				m_ParticlesVector.push_back(po);
			}

			LOG_INFO(m_ParticlesVector);
		}

		ParticleEmitter(Texture* texture)
			: m_Texture(texture)
		{
			ParticleEmitter();
		}
	
		int FindUnusedParticle()
		{
			for (int i = m_LastUnusedParticle; i < MAX_PARTICLE_COUNT; i++)
			{
				if (!m_ParticlesVector[i]->m_Active)
				{
					m_LastUnusedParticle = i;
					return m_LastUnusedParticle;
				}
			}

			for (int i = 0; i < m_LastUnusedParticle; i++)
			{
				if (!m_ParticlesVector[i]->m_Active)
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

			m_ParticlesVector[FindUnusedParticle()] = new ParticleObject(startTranslationVelocity, startRotationVelocity, startScaleVelocity);
		}

		std::vector<glm::mat4> GetAllTransforms()
		{
			LOG_INFO("GetAllTransforms() ");

			std::vector<glm::mat4> transforms;
			transforms.reserve(MAX_PARTICLE_COUNT);

			for (int i = 0; i < MAX_PARTICLE_COUNT; i++)
			{
				if (m_ParticlesVector[i]->m_Active)
					transforms.push_back(glm::mat4(1.0f));//m_Particles[i].GetTransform());
			}

			return transforms;
		}

		

	};
}
	