#pragma once

#include "entityx/System.h"
#include "Gameplay/Components/TestTransform.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace sixengine {

    class RotationSystem : public entityx::System<RotationSystem>
    {
        void update(entityx::EntityManager& es, entityx::EventManager& events, entityx::TimeDelta dt) override
        {
            entityx::ComponentHandle<TestTransform> transform;

            for (entityx::Entity entity : es.entities_with_components(transform)) {
                transform = entity.component<TestTransform>();
                transform.get()->transform = glm::rotate(transform.get()->transform, glm::radians(20.0f * dt), glm::vec3(0.0f, 1.0f, 0.0f));
            };
        }
    };

}
