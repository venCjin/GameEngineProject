#pragma once
#include <ECS/SystemManager.h>
#include <Gameplay\Components\Transform.h>
#include <Gameplay\Components\UIRoot.h>

namespace sixengine {
    struct UIRendererSystem : public System
    {
    public:
	    void Update(EntityManager& es, EventManager& events, float dt) override
	    {

            ComponentHandle<Transform> transform;
            ComponentHandle<UIRoot> root;

            for (Entity entity : es.EntitiesWithComponents<Transform, UIRoot>())
            {            

                transform = entity.Component<Transform>();
                root = entity.Component<UIRoot>();
                float yaw = root->m_Camera->GetYaw();
                float pitch = root->m_Camera->GetPitch();
                glm::mat4 newTransform(1.0f);




               // glm::mat4 viewMatrix = root->m_Camera->GetViewMatrix();



                //glm::vec3 translation = transform->getWorldPosition() - glm::vec3(0, 0, 3);
                //newTransform = glm::translate(newTransform, translation);
                //newTransform[0] = glm::vec4(viewMatrix[0][0], viewMatrix[1][0], viewMatrix[2][0], 0.0f);
                //newTransform[1] = glm::vec4(viewMatrix[0][1], viewMatrix[1][1], viewMatrix[2][1], 0.0f);
                //newTransform[2] = glm::vec4(viewMatrix[0][2], viewMatrix[1][2], viewMatrix[2][2], 0.0f);
                //newTransform[3] = glm::vec4(viewMatrix[0][3], viewMatrix[1][3], viewMatrix[2][3], 0.0f);

                
                std::stringstream stream;
                stream << "cam pos: " << transform->getWorldPosition().x << " " << transform->getWorldPosition().y << " " << transform->getWorldPosition().z;
                LOG_CORE_INFO(stream.str());
                stream.flush();
                //transform->SetLocal(newTransform);
            }
    
	    }
    };
}
