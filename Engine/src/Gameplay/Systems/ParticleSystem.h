#pragma once

#include <ECS/SystemManager.h>

#include <Gameplay/Components/ParticleEmitter.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>

namespace sixengine {
	SYSTEM(ParticleSystem, ParticleEmitter)
	{
		float particlesToInit = 0.0f;
		float timer = 0.0f;

		void Update(EventManager & eventManager, float dt) override
		{
			if (timer <= m_ParticleEmitter->m_EmissionDuration)
			{
				particlesToInit += m_ParticleEmitter->m_EmissionFrequency * dt;

				if (particlesToInit >= 1.0f)
				{
					float wholeNumberOfParticles = floor(particlesToInit);
					particlesToInit -= wholeNumberOfParticles;

					for (int i = 0; i < (int)wholeNumberOfParticles; i++)
					{
						m_ParticleEmitter->SpawnParticle();
					}
				}
			}
			else if (m_ParticleEmitter->m_Loop)
				timer = 0.0f;

			timer += dt;

			for (int i = 0; i < MAX_PARTICLE_COUNT; i++)
			{
				if (m_ParticleEmitter->m_Particles[i].m_Timer <= m_ParticleEmitter->m_ParticleLifeDuration)
				{
					m_ParticleEmitter->m_Particles[i].m_Timer += dt;
					m_ParticleEmitter->m_Particles[i].m_TranslationVelocity += m_ParticleEmitter->m_TranslationAcceleration * dt;
					m_ParticleEmitter->m_Particles[i].UpdateTransform(dt);
				}

				else
					m_ParticleEmitter->m_Particles[i].m_Active = false;
			}

			
		}

		
	};
}
