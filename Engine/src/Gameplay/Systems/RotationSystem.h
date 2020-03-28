#pragma once

#include "entityx/System.h"
#include "Gameplay/Components/TestTransform.h"
#include "Gameplay/Components/TestRotation.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace sixengine {

    class RotationSystem : public entityx::System<RotationSystem>
    {
        void update(entityx::EntityManager& es, entityx::EventManager& events, entityx::TimeDelta dt) override
        {
            entityx::ComponentHandle<TestTransform> transform;
            entityx::ComponentHandle<TestRotation> rotation;

            for (entityx::Entity entity : es.entities_with_components(transform, rotation)) 
            {
                transform = entity.component<TestTransform>();
                transform.get()->transform = glm::rotate(transform.get()->transform, glm::radians(rotation->speed * dt), rotation->axis);
            }
        }
    };

}
