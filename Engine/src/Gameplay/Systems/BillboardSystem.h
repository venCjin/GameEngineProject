#pragma once

#include <ECS/SystemManager.h>
#include <Gameplay/Components/Transform.h>
#include <Gameplay/Components/TestRotation.h>
#include <Gameplay/Components/Billboard.h>

namespace sixengine {

	class BillbordSystem : public System
	{
		void Update(EntityManager& es, EventManager& events, float dt) override
		{
			ComponentHandle<Transform> transform;
			ComponentHandle<Billboard> camera;
			for (Entity entity : es.EntitiesWithComponents<Transform, Billboard>())
			{
				transform = entity.Component<Transform>();
				camera = entity.Component<Billboard>();
				glm::mat4 viewMatrix = camera->m_Camera->GetViewMatrix();

				glm::mat4 matrix(1.0f);
				matrix = glm::translate(matrix, transform->getWorldPosition());
				matrix[0] = glm::vec4(viewMatrix[0][0], viewMatrix[1][0], viewMatrix[2][0], 0.0f);
				matrix[1] = glm::vec4(viewMatrix[0][1], viewMatrix[1][1], viewMatrix[2][1], 0.0f);
				matrix[2] = glm::vec4(viewMatrix[0][2], viewMatrix[1][2], viewMatrix[2][2], 0.0f);

				
				transform->SetWorld(matrix);
			}
		}
	};

}
