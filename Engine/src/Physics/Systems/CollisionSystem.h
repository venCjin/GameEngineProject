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

		static Entity* CheckSphere(glm::vec3 center, float radius);
		static std::vector<Entity*> CheckSphereAll(glm::vec3 center, float radius);
	};
	
	static int RayCast(glm::vec3 p, glm::vec3 d);
}
