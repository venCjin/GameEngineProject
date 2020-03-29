#pragma once

#include "entityx/System.h"
#include "Gameplay/Components/Transform.h"
#include "Gameplay/Components/TestRotation.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace sixengine {

    class RotationSystem : public entityx::System<RotationSystem>
    {
        void update(entityx::EntityManager& es, entityx::EventManager& events, entityx::TimeDelta dt) override
        {
            entityx::ComponentHandle<Transform> transform;
            entityx::ComponentHandle<TestRotation> rotation;

            for (entityx::Entity entity : es.entities_with_components(transform, rotation)) 
            {
                transform = entity.component<Transform>();
                transform->SetWorld(glm::rotate(transform->GetWorld(), glm::radians(rotation->speed * dt), rotation->axis));
            }
        }
    };

}
