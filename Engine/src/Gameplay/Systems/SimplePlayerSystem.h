#pragma once

#include <ECS/SystemManager.h>

#include <Gameplay/Components/Transform.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "Core/CameraSystem/Camera.h"
#include <Physics\Components\DynamicBody.h>
#include <Physics/Systems/CollisionSystem.h>

namespace sixengine {
	

	SYSTEM(SimplePlayerSystem, Transform, SimplePlayer, Material)
	{
		//bool m_OnSurface = true;
		float speed = .25f;
		float maxSpeed = 1.0f;
		float playerHeight = 2.0f;
		float health = 100.0f;
		//float air = 100.0f;
		float airLosingRate = 5.0f;
		void Update(EventManager & eventManager, float dt) override
		{
			//LOG_INFO(m_SimplePlayer->collider->IsStatic());
			glm::vec3 dir = glm::vec3();
			DynamicBody* _db = m_SimplePlayer->gameObject->GetComponent<DynamicBody>().Get();	
			_db->m_Drag = 4;
			glm::vec3 cameraDir = -glm::normalize(glm::vec3(Camera::ActiveCamera->m_Transform->GetForward().x, 0.0f, Camera::ActiveCamera->m_Transform->GetForward().z));
			

			glm::vec3 f = m_SimplePlayer->transform->GetForward();
			LOG_WARN("Forward: {0} {1} {2}", f.x, f.y, f.z);

			if (Input::IsMouseButtonActive(0))
			{
				LOG_INFO("___MOUSE");
				Entity* a = CollisionSystem::CheckSphere(m_SimplePlayer->transform->GetWorldPosition() + m_SimplePlayer->transform->GetForward() * 1.0f, 1.0f);
				if (a != nullptr)
					LOG_ERROR("HIT COLLIDER");
			}

			if (Input::IsKeyActive(KeyCode::DOWN)) 
			{ 
				_db->m_Velocity += cameraDir * speed; 
				
				

			}
			if (Input::IsKeyActive(KeyCode::UP)) { 
				_db->m_Velocity -= cameraDir * speed; 

				
			}
			cameraDir = glm::rotateY(cameraDir, glm::radians(90.0f));
			if (Input::IsKeyActive(KeyCode::RIGHT)) { _db->m_Velocity += cameraDir * speed; }
			if (Input::IsKeyActive(KeyCode::LEFT)) { _db->m_Velocity -= cameraDir * speed; }
			//glm::clamp(_db->m_Velocity, glm::vec3(0.0f), glm::vec3(maxSpeed));
			//if (Input::IsKeyActive(KeyCode::PAGE_UP)) { dir.y += 1.0f; }
			//if (Input::IsKeyActive(KeyCode::PAGE_DOWN)) { dir.y -= 1.0f; }
			if (glm::dot(_db->m_Velocity, _db->m_Velocity) > 0.001f) 
			{
				m_Transform->LookAt((m_Transform->GetWorldPosition() - _db->m_Velocity));
			}
			//LOG_CORE_INFO("Speed {0}", speed);
			//LOG_CORE_INFO("CAmera position {0} {1} {2}", Camera::ActiveCamera->m_Transform->GetWorldPosition().x, Camera::ActiveCamera->m_Transform->GetWorldPosition().y, Camera::ActiveCamera->m_Transform->GetWorldPosition().z);
			LOG_CORE_INFO("CAmera direction {0} {1} {2}", cameraDir.x, cameraDir.y, cameraDir.z);
			//LOG_CORE_INFO("Dot {0}", glm::dot(_db->m_Velocity, _db->m_Velocity));
			if (Input::IsKeyPressed(KeyCode::SPACE))
			{
				m_SimplePlayer->m_OnSurface = !m_SimplePlayer->m_OnSurface;
				if (m_SimplePlayer->m_OnSurface)
				{
					m_Material->SetShader(MaterialManager::getInstance()->Get("parasiteZombie")->GetShader());
					m_Transform->SetWorldPosition(m_Transform->GetWorldPosition() - glm::vec3(0.0f, -playerHeight, 0.0f));

				}
				else
				{
					m_Material->SetShader(MaterialManager::getInstance()->Get("Transparent")->GetShader());
					m_Transform->SetWorldPosition(m_Transform->GetWorldPosition() - glm::vec3(0.0f, +playerHeight, 0.0f));
				}
			}

			if (m_SimplePlayer->m_OnSurface)
			{
				m_SimplePlayer->air += Timer::Instance()->DeltaTime()*airLosingRate;
			}
			else
			{
				m_SimplePlayer->air -= Timer::Instance()->DeltaTime()*airLosingRate;
			}
			m_SimplePlayer->air = glm::clamp<float>(m_SimplePlayer->air, 0.0f, 100.0f);

			/*auto originalPos = m_Transform->GetWorldPosition();

			glm::vec3 finalDir = m_Transform->getWorldPosition();
			finalDir -= dir.z * Camera::ActiveCamera->m_Transform->GetForward() * dt * 5.0f;
			finalDir += dir.x * Camera::ActiveCamera->m_Transform->GetRight() * dt * 5.0f;
			finalDir.y = m_Transform->getWorldPosition().y;

			m_Transform->SetWorldPosition(finalDir);

			if (glm::dot(finalDir - originalPos, finalDir - originalPos) > 0)
			{
				m_Transform->LookAt(m_Transform->GetWorldPosition() + (originalPos - finalDir));
			}*/
		}
	};
}
