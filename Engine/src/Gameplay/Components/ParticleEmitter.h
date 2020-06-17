#pragma once
#include "pch.h"

#include <glm/glm.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Core/ISerializable.h"
#include "Gameplay/ParticleObject.h"

#include "Renderer/Texture.h"
#include "Core/CameraSystem/Camera.h"

#include <iostream>     // std::cout
#include <algorithm>    // std::sort
#include <vector>       // std::vector


#define MAX_PARTICLE_COUNT 500

namespace sixengine {

	class ParticleEmitter// : public ISerializable
	{
	private:
		int m_LastUnusedParticle = 0;
	public:
		bool m_Started = true;

		float m_EmissionDuration = 2.0f;
		float m_EmissionFrequency = 10.0f; // in Hertz, so number of particles per second
		
		bool m_Loop = true;
		float m_ParticleLifeDuration = 3.0f;
		
		float m_StartSpeed = 2.0f; 
		// m_StartSize?

		float m_MaxDirectionAngle = 30.0f;

		float m_TranslationAcceleration = 0.0f;
		float m_RotationAcceleration = 0.0f;
		float m_SizeAcceleration = 0.0f;
		ParticleObject m_Particles[MAX_PARTICLE_COUNT];

		Texture* m_Texture;


		ParticleEmitter() {
			srand(static_cast <unsigned> (time(0)));			
		};

		ParticleEmitter(float emissionDuration, float emissionFreq, bool loop, float particleLifeDuration, float startSpeed, float maxAngle, Texture* texture) 
			: m_EmissionDuration(emissionDuration), m_EmissionFrequency(emissionFreq), m_Loop(loop), m_ParticleLifeDuration(particleLifeDuration), m_StartSpeed(startSpeed), m_MaxDirectionAngle(maxAngle), m_Texture(texture)
		{ 
			ParticleEmitter();
		}

		ParticleEmitter(Texture* texture)
			: m_Texture(texture)
		{
			ParticleEmitter();
		}
	
		void Start()
		{
			m_Started = true;
		}

		void Stop()
		{
			m_Started = false;
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
			//glm::mat4 rotation = glm::mat4(1.0f);
			//rotation *= glm::yawPitchRoll(45.0f, 0.0f, 89.0f);

			float randomX = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			float randomY = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			float randomZ = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

			glm::quat q = glm::quat(glm::vec3(glm::radians(m_MaxDirectionAngle), glm::radians(0.0f), glm::radians(0.0f)));
			glm::vec3 direction = glm::vec3(0.0f, 1.0f, 0.0f);
			direction = glm::normalize(q * direction);

			//std::cout << "direction.x: " << direction.x << " direction.y: "<< direction.y << " direction.z: " << direction.z << std::endl;
			if (m_MaxDirectionAngle > 359.0f)
				direction = glm::vec3(randomX * 2.0f - 1.0f, randomY * 2.0f - 1.0f, randomZ * 2.0f - 1.0f);
			else
				direction = glm::vec3(direction.z * (randomX * 2.0f - 1.0f), 1.0f, direction.z * (randomZ * 2.0f - 1.0f));

			direction = glm::normalize(direction);
			
			glm::vec3 startTranslationVelocity = direction * m_StartSpeed;
			glm::vec3 startRotationVelocity = glm::vec3(0.0f);
			glm::vec3 startScaleVelocity = glm::vec3(0.0f);

			m_Particles[FindUnusedParticle()] = ParticleObject(startTranslationVelocity, startRotationVelocity, startScaleVelocity);
		}



		std::vector<ParticleData> GetAllParticleData()
		{
			std::vector<ParticleData> particleDataVector;
		
			particleDataVector.resize(MAX_PARTICLE_COUNT);

			for (int i = 0; i < MAX_PARTICLE_COUNT; i++)
			{
				if (m_Particles[i].m_Active)
					particleDataVector[i] = m_Particles[i].GetParticleData();
			}

			std::sort(particleDataVector.begin(), particleDataVector.end(),
				[](const ParticleData& a, const ParticleData& b) -> bool
				{
					glm::vec3 posA(a.transformMatrix[3]);
					glm::vec3 posB(b.transformMatrix[3]);

					glm::vec3 cameraPosition = Camera::ActiveCamera->m_Transform->GetWorldPosition();

					return glm::length(cameraPosition - posA) > glm::length(cameraPosition - posB);
				});

			return particleDataVector;
		}


		

	};
}
	