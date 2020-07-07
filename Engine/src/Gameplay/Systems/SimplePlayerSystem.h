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
#include <Core/Application.h>
#include <Core/Scene.h>
#include <Physics/Systems/CollisionSystem.h>
#include <Physics/Components/DynamicBody.h>
#include <Core/AudioManager.h>
#include <AI/Enemy.h>
#include <Gameplay/Components/Generator.h>
#include <Gameplay/Components/UndergroundObject.h>
#include <Gameplay/Components/ScolopendraComponent.h>
#include <Gameplay/Components/DestroyableWall.h>
#include <Gameplay/Components/ConcreteFloor.h>

namespace sixengine {
	
	SYSTEM(SimplePlayerSystem, Transform, SimplePlayer, Material)
	{
		//bool m_OnSurface = true;
		float speed = .525f;
		//float maxSpeed = 2.0f;
		float speedMultiplicator = 1.4f;
		float playerHeight = 1.0f;
		//float air = 100.0f;
		float airLosingRate = 16.0f;

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
				//glm::vec3 pos = collisionEvent.m_Entity.Component<Transform>()->GetWorldPosition();
				//collisionEvent.m_Entity.Component<Transform>()->SetWorldPosition(glm::vec3(pos.x, 0.1f, pos.z));
				collisionEvent.m_Entity.Component<SimplePlayer>()->m_Health -= 10.0f;
				//Application::Get().GetScene()->m_BatchRenderer->SetShake(true);
			}
		}

	public:
		bool CanChangeMode()
		{
			auto pos = m_SimplePlayer->transform->GetWorldPosition();

			pos.y = 0.0f;
			
			for (auto hit : CollisionSystem::CheckSphereAll(pos, 1.0f))
			{
				if (Entity::Valid(hit.GetID()))
				{
					if (hit.HasComponent<ConcreteFloor>())
					{
						return false;
					}
				}
			}

			return true;
		}

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

				glm::vec3 center = m_SimplePlayer->transform->GetWorldPosition() - m_SimplePlayer->transform->GetForward();
				center.y = 0.0f;
				for (auto hit : CollisionSystem::CheckSphereAll(center, 1.25f))
				{
					if (Entity::Valid(hit.GetID()))
					{
						if (hit.HasComponent<Enemy>())
						{
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

			//hackerman
			if (Input::IsKeyActive(KeyCode::H))
			{
				m_SimplePlayer->m_Health = 100.0f;
			}

			//koniec hackerman

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
			if (Input::IsKeyPressed(KeyCode::SPACE) && CanChangeMode())
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

					SpawnHole();
					BatchRenderer::Instance()->SetBlur(false);

					/*for (auto obj : Application::Get().GetEntityManager()->EntitiesWithComponents<Mesh>())
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
					}*/
					for (auto obj : Application::Get().GetEntityManager()->EntitiesWithComponents<Mesh, ParticleEmitter, Enemy>())
					{
						obj.Component<Mesh>()->m_Visible = true;
						obj.Component<ParticleEmitter>()->Stop();
					}
					for (auto obj : Application::Get().GetEntityManager()->EntitiesWithComponents<Mesh, UndergroundObject>())
					{
						obj.Component<Mesh>()->m_Visible = false;
					}
				}
				else
				{
					SpawnHole();
					speed *= speedMultiplicator;
					m_SimplePlayer->scolopendraMaterial->SetShader(MaterialManager::getInstance()->Get("TransparentMaterial")->GetShader());
					m_Transform->SetWorldPosition(m_Transform->GetWorldPosition() - glm::vec3(0.0f, +playerHeight, 0.0f));
					m_SimplePlayer->MixingCamera->SetTargetCamera(m_SimplePlayer->UnderSurfaceCamera);

					BatchRenderer::Instance()->SetBlur(true);
					
					/*for (auto obj : Application::Get().GetEntityManager()->EntitiesWithComponents<Mesh>())
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
					}*/
					for (auto obj : Application::Get().GetEntityManager()->EntitiesWithComponents<Mesh, ParticleEmitter, Enemy>())
					{
						obj.Component<Mesh>()->m_Visible = false;
						obj.Component<ParticleEmitter>()->Start();
					}
					for (auto obj : Application::Get().GetEntityManager()->EntitiesWithComponents<Mesh, UndergroundObject>())
					{
						obj.Component<Mesh>()->m_Visible = true;
					}
				}
			}

			if (m_SimplePlayer->m_OnSurface)
			{
				m_SimplePlayer->m_Air += Timer::Instance()->DeltaTime() * airLosingRate;
			}
			else
			{
				m_SimplePlayer->m_Air -= Timer::Instance()->DeltaTime() * airLosingRate;
			}
			m_SimplePlayer->m_Air = glm::clamp<float>(m_SimplePlayer->m_Air, 0.0f, 100.0f);

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

		void SpawnHole()
		{
			GameObject* obj = new GameObject(*Application::Get().GetEntityManager());
			obj->AddComponent<Transform>(obj);
			glm::vec3 pos = m_Transform->GetWorldPosition();
			obj->GetComponent<Transform>()->SetWorldPosition(glm::vec3(pos.x, pos.y - 0.13f, pos.z));
			obj->GetComponent<Transform>()->SetLocalScale(1.0f, 1.0f, 1.0f);
			obj->AddComponent<Material>(*Application::Get().GetScene()->m_MaterialManager->Get("GroundPBR"));
			obj->AddComponent<Mesh>(Application::Get().GetScene()->m_ModelManager->GetModel("Kopiec"));

			Application::Get().GetScene()->m_SceneRoot->AddChild(obj);
		}
	};
}
