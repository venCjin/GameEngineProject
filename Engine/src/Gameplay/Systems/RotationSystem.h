#pragma once

#include <ECS/SystemManager.h>
#include <Gameplay/Components/Transform.h>
#include <Gameplay/Components/TestRotation.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace sixengine {

    class RotationSystem : public System
    {
        void Update(EntityManager& es, EventManager& events, float dt) override
        {
            ComponentHandle<Transform> transform;
            ComponentHandle<TestRotation> rotation;

            for (Entity entity : es.EntitiesWithComponents<Transform, TestRotation>()) 
            {
                transform = entity.Component<Transform>();
                rotation = entity.Component<TestRotation>();

                transform->SetWorld(glm::rotate(transform->GetWorld(), glm::radians(rotation->speed * dt), rotation->axis));

            }
        }
    };

}
