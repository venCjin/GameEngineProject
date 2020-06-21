#pragma once

#include "Gameplay/Components/Transform.h"
#include "Gameplay/Components/Rotation.h"
#include <Gameplay/Components/Billboard.h>

#include <Physics/Components/BoxCollider.h>
#include <Physics/Components/SphereCollider.h>

#include <Physics/Components/DynamicBody.h>
#include <Physics/Components/StaticBody.h>

#include "Physics/Collision.h"

namespace sixengine {

	struct OnCollision : BaseEvent
	{
	public:
		Collision collision;

		OnCollision(Entity entity, Collision collision)
			: BaseEvent(entity), collision(collision) {}
	};

	class CollisionSystem : public BaseSystem
	{
	private:
		std::vector<Entity> m_BoxEntities;
		std::vector<Entity> m_SphereEntities;
		EventManager* m_EventManager;

	protected: 
		Entity entity; 
		ComponentHandle<Transform> m_Transform; 
		ComponentHandle<BoxCollider> m_BoxCollider;		

	public: 
		void GetEntities(EntityManager& entityManager) override final 
		{
			m_BoxEntities = entityManager.EntitiesWithComponents<Transform, BoxCollider>();
			m_SphereEntities = entityManager.EntitiesWithComponents<Transform, SphereCollider>();
		}

		void BoxWithBox(Entity entityA , Entity entityB);
		void SphereWithSphere(Entity circleA, Entity circleB);
		void BoxWithCircle(Entity box, Entity circle);

		void UpdateBoxesWithBoxes();
		void UpdateSpheresWithSpheres();
		void UpdateBoxesWithSpheres();

		void HandleCollision(Entity collider, class Collision collision);
		
		void UpdateAll(EventManager& eventManager, float dt) override final;
		void Update(EventManager& eventManager, float dt) override {}

		static glm::vec3 ClosestPointOnSphere(glm::vec3 spherePos, float sphereRad, glm::vec3 point);
		static int TestRaySphere(glm::vec3 p, glm::vec3 d, Entity entity);
		static int TestSegmentSphere(glm::vec3 v, glm::vec3 w, Entity entity);
		static int TestSegmentAABB(glm::vec3 p0, glm::vec3 p1, Entity entity);
		static std::vector<Entity> RaycastAll(glm::vec3 origin, glm::vec3 direction);

		static Entity CheckSphere(glm::vec3 center, float radius);
		static std::vector<Entity> CheckSphereAll(glm::vec3 center, float radius);
	};
}
