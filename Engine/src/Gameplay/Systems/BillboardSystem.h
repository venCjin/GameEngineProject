#pragma once

#include <ECS/SystemManager.h>
#include <ECS/EventManager.h>

#include <Gameplay/Components/Transform.h>
#include <Gameplay/Components/TestRotation.h>
#include <Gameplay/Components/Billboard.h>

namespace sixengine {

	SYSTEM(BillboardSystem, Transform, Billboard)
	{
	public:
		void handler(BaseEvent &e)
		{
			OnTestEvent& d = dynamic_cast<OnTestEvent&>(e);
		}

		void OnStart(EventManager& eventManager) override
		{
			eventManager.AddListener<OnTestEvent>(&BillboardSystem::handler, this);
		}

		void Update(EventManager& eventManager, float dt) override
		{
			glm::mat4 viewMatrix = m_Billboard->m_Camera->GetViewMatrix();

			glm::mat4 matrix(1.0f);
			matrix = glm::translate(matrix, m_Transform->getWorldPosition());
			matrix[0] = glm::vec4(viewMatrix[0][0], viewMatrix[1][0], viewMatrix[2][0], 0.0f);
			matrix[1] = glm::vec4(viewMatrix[0][1], viewMatrix[1][1], viewMatrix[2][1], 0.0f);
			matrix[2] = glm::vec4(viewMatrix[0][2], viewMatrix[1][2], viewMatrix[2][2], 0.0f);
				
			m_Transform->SetWorld(matrix);
		}
	};

}
