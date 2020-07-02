#pragma once

#include <ECS/SystemManager.h>

#include <Gameplay/Components/Transform.h>
#include <Gameplay/Components/SimplePlayer.h>
#include <Gameplay/Components/Collectable.h>
#include <Gameplay/Components/Pistol.h>
#include <Gameplay/Components/Projectile.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include <Core/CameraSystem/Camera.h>
#include <Physics/Systems/CollisionSystem.h>
#include <Physics/Components/DynamicBody.h>
#include <Core/AudioManager.h>
#include <AI/Enemy.h>
#include <Gameplay/Components/Generator.h>
#include <Gameplay/Components/UndergroundObject.h>
#include <Gameplay/Components/ScolopendraComponent.h>
#include <Gameplay/Components/DestroyableWall.h>

namespace sixengine {
	
	SYSTEM(SimplePlayerSystem, Transform, SimplePlayer, Material)
	{
		//bool m_OnSurface = true;
		float speed = .625f;
		//float maxSpeed = 2.0f;
		float speedMultiplicator = 2.0f;
		float playerHeight = 1.0f;
		//float air = 100.0f;
		float airLosingRate = 5.0f;

		// Going Underground
		/*float speed = 2.0f;
		float length = 3.0f;
		glm::vec3 startPos = glm::vec3(0.0f);
		float duration = 1.0f / speed;
		bool movingVertically = false;*/

		float timer = 0.0f;
		float cooldown = .5f;
		void OnCollisionHandle(BaseEvent & e)
		{
			OnCollision& collisionEvent = dynamic_cast<OnCollision&>(e);

			if (collisionEvent.m_Entity.HasComponent<SimplePlayer>() &&
				collisionEvent.collision.other.HasComponent<Collectable>())
			{
				//collisionEvent.collision.other.RemoveComponent<BoxCollider>();
				//collisionEvent.collision.other.RemoveComponent<Mesh>();
				collisionEvent.collision.other.Component<Transform>()->Translate(glm::vec3(0.0f, -100.0f, 0.0f));
				collisionEvent.m_Entity.Component<SimplePlayer>()->m_Health += collisionEvent.collision.other.Component<Collectable>()->hp;
				if (collisionEvent.m_Entity.Component<SimplePlayer>()->m_Health > 100.0f) collisionEvent.m_Entity.Component<SimplePlayer>()->m_Health = 100.0f;
			}
			else if (collisionEvent.m_Entity.HasComponent<SimplePlayer>() &&
				collisionEvent.collision.other.HasComponent<Projectile>())
			{
				collisionEvent.collision.other.Component<Transform>()->Translate(glm::vec3(0.0f, -100.0f, 0.0f));
				glm::vec3 pos = collisionEvent.m_Entity.Component<Transform>()->GetWorldPosition();
				collisionEvent.m_Entity.Component<Transform>()->SetWorldPosition(glm::vec3(pos.x, 0.1f, pos.z));
				collisionEvent.m_Entity.Component<SimplePlayer>()->m_Health -= 10.0f;
			}
		}

	public:
		void OnStart(EventManager & eventManager) override
		{
			eventManager.AddListener<OnCollision>(&SimplePlayerSystem::OnCollisionHandle, this);
		}

		void Update(EventManager & eventManager, float dt) override
		{
			//LOG_INFO(m_SimplePlayer->collider->IsStatic());



			//AUDIO
			//Updating audio listener 
			AudioManager::getInstance()->SetListenerPosition(m_Transform->GetWorldPosition(), m_Transform->GetForward());
			//AUDIO
			glm::vec3 dir = glm::vec3();
			DynamicBody* _db = m_SimplePlayer->gameObject->GetComponent<DynamicBody>().Get();	
			_db->m_Drag = 4;
			glm::vec3 cameraDir = -glm::normalize(glm::vec3(Camera::ActiveCamera->m_Transform->GetForward().x, 0.0f, Camera::ActiveCamera->m_Transform->GetForward().z));
			timer += dt;

			glm::vec3 f = m_SimplePlayer->transform->GetForward();
			//LOG_WARN("Forward: {0} {1} {2}", f.x, f.y, f.z);

			if (Input::IsMouseButtonActive(0) && timer > cooldown && m_SimplePlayer->m_OnSurface)
			{
				//LOG_INFO("___MOUSE");
				timer = 0.0f;
				m_SimplePlayer->scolopendraAnimation->ChangeAnimation(std::string("attack"));

				glm::mat4 model = glm::mat4(1.0f);
				model = glm::translate(model, m_SimplePlayer->transform->GetWorldPosition() - m_SimplePlayer->transform->GetForward());
				Application::attack->model = model;

				for (auto hit : CollisionSystem::CheckSphereAll(m_SimplePlayer->transform->GetWorldPosition() - m_SimplePlayer->transform->GetForward(), 1.0f))
				{
					if (Entity::Valid(hit.GetID()))
					{
						LOG_INFO(hit.HasComponent<DestroyableWall>());

						if (hit.HasComponent<Enemy>())
						{
							//LOG_INFO("MAM CIE CHUJU!");
							hit.Component<Enemy>()->ReceiveDamage(50.0f);
						}
						else if (hit.HasComponent<Generator>())
						{
							hit.Component<Generator>()->DestroyGenerator();
						}
						else if (hit.HasComponent<DestroyableWall>())
						{
							hit.Component<DestroyableWall>()->Destroy();
						}
					}
				}
			}

			if (Input::IsKeyActive(KeyCode::S)) 
			{ 
				_db->m_Velocity += cameraDir * speed; 
				
				

			}
			if (Input::IsKeyActive(KeyCode::W)) { 
				_db->m_Velocity -= cameraDir * speed; 

				
			}
			cameraDir = glm::rotateY(cameraDir, glm::radians(90.0f));
			if (Input::IsKeyActive(KeyCode::D)) { _db->m_Velocity += cameraDir * speed; }
			if (Input::IsKeyActive(KeyCode::A)) { _db->m_Velocity -= cameraDir * speed; }
			//glm::clamp(_db->m_Velocity, glm::vec3(0.0f), glm::vec3(maxSpeed));
			//if (Input::IsKeyActive(KeyCode::PAGE_UP)) { dir.y += 1.0f; }
			//if (Input::IsKeyActive(KeyCode::PAGE_DOWN)) { dir.y -= 1.0f; }
			if (glm::dot(_db->m_Velocity, _db->m_Velocity) > 0.001f) 
			{
				m_Transform->LookAt((m_Transform->GetWorldPosition() - _db->m_Velocity));
				// check if undergorund or not
				if (m_SimplePlayer->m_OnSurface)
					m_SimplePlayer->scolopendraAnimation->ChangeAnimation(std::string("walk"));
				else
					m_SimplePlayer->scolopendraAnimation->ChangeAnimation(std::string("walk_ug"));

			}
			else
			{
				if (m_SimplePlayer->m_OnSurface)
					m_SimplePlayer->scolopendraAnimation->ChangeAnimation(std::string("idle"));
				else
					m_SimplePlayer->scolopendraAnimation->ChangeAnimation(std::string("idle_ug"));
			}
			//LOG_CORE_INFO("Speed {0}", speed);
			//LOG_CORE_INFO("CAmera position {0} {1} {2}", Camera::ActiveCamera->m_Transform->GetWorldPosition().x, Camera::ActiveCamera->m_Transform->GetWorldPosition().y, Camera::ActiveCamera->m_Transform->GetWorldPosition().z);
			//LOG_CORE_INFO("CAmera direction {0} {1} {2}", cameraDir.x, cameraDir.y, cameraDir.z);
			//LOG_CORE_INFO("Dot {0}", glm::dot(_db->m_Velocity, _db->m_Velocity));
			if (Input::IsKeyPressed(KeyCode::SPACE))
			{
				/*if (!movingVertically)
				{

					movingVertically = true;
				}*/

				m_SimplePlayer->m_OnSurface = !m_SimplePlayer->m_OnSurface;
				if (m_SimplePlayer->m_OnSurface)
				{
					speed /= speedMultiplicator;
					m_SimplePlayer->scolopendraMaterial->SetShader(MaterialManager::getInstance()->Get("GreenAnim")->GetShader());
					m_Transform->SetWorldPosition(m_Transform->GetWorldPosition() - glm::vec3(0.0f, -playerHeight, 0.0f));
					m_SimplePlayer->MixingCamera->SetTargetCamera(m_SimplePlayer->OnSurfaceCamera);

					BatchRenderer::Instance()->SetBlur(false);

					for (auto obj : Application::Get().GetEntityManager()->EntitiesWithComponents<Mesh>())
					{
						obj.Component<Mesh>()->m_Visible = true;

						if (obj.HasComponent<ParticleEmitter>())
						{
							if (obj.HasComponent<Enemy>())
							{
								obj.Component<ParticleEmitter>()->Stop();
							}
							else
							{
								// turn on stars particle, cause bug with enemies particles
								//obj.Component<ParticleEmitter>()->Start();
							}
						}
						else if (obj.HasComponent<UndergroundObject>())
						{
							obj.Component<Mesh>()->m_Visible = false;
						}
					}
					
				}
				else
				{
					speed *= speedMultiplicator;
					m_SimplePlayer->scolopendraMaterial->SetShader(MaterialManager::getInstance()->Get("TransparentMaterial")->GetShader());
					m_Transform->SetWorldPosition(m_Transform->GetWorldPosition() - glm::vec3(0.0f, +playerHeight, 0.0f));
					m_SimplePlayer->MixingCamera->SetTargetCamera(m_SimplePlayer->UnderSurfaceCamera);
					
					BatchRenderer::Instance()->SetBlur(true);
					
					for (auto obj : Application::Get().GetEntityManager()->EntitiesWithComponents<Mesh>())
					{
						obj.Component<Mesh>()->m_Visible = false;

						if (obj.Component<Mesh>()->GetModel()->GetName().compare("Ground.obj")==0)
						{
							obj.Component<Mesh>()->m_Visible = true;
						}
						else if (obj.HasComponent<ParticleEmitter>())
						{
							if (obj.HasComponent<Enemy>())
							{
								obj.Component<ParticleEmitter>()->Start();
							}
							else
							{
								// turn off stars particle, cause bug with enemies particles
								//obj.Component<ParticleEmitter>()->Stop();
							}
						}
						else if (obj.HasComponent<UndergroundObject>())
						{
							obj.Component<Mesh>()->m_Visible = true;
						}
						else if (obj.HasComponent<ScolopendraComponent>())
						{
							obj.Component<Mesh>()->m_Visible = true;
						}
					}

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

		float EaseInCubic(float x)
		{
			return x < 0.5f ? 4.0f * x * x * x : 1.0f - pow(-2.0f * x + 2.0f, 3.0f) / 2.0f;
		}
	};
}
