#pragma once

#include "Gameplay/Components/Transform.h"
#include "Gameplay/Components/TestRotation.h"
#include <Gameplay/Components/Billboard.h>

#include <Physics/Components/BoxCollider.h>
#include <Physics/Components/SphereCollider.h>

namespace sixengine {

	class CollisionSystem : public BaseSystem
	{
	private: 
		std::vector<Entity> m_BoxEntities;
		std::vector<Entity> m_SphereEntities;

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
	};
}