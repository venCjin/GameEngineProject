#pragma once

#include <ECS/SystemManager.h>

#include <Gameplay/Components/Transform.h>
#include <Gameplay/Components/TestRotation.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace sixengine {

	struct OnTestEvent : BaseEvent
	{
	public:
		bool a;

		OnTestEvent(Entity::ID id, bool a)
			: BaseEvent(id), a(a) {}
	};

	SYSTEM(RotationSystem, Transform, TestRotation)
    {
		void Update(EventManager& eventManager, float dt) override
		{
			m_Transform->SetWorld(glm::rotate(m_Transform->GetWorld(), glm::radians(m_TestRotation->speed * dt), m_TestRotation->axis));

			if (m_TestRotation->speed > 3)
			{
				eventManager.Emit(OnTestEvent(entity.GetID(), true));
			}
		}
    };
}
