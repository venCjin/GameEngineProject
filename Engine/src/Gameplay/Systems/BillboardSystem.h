#pragma once

#include <ECS/SystemManager.h>
#include <ECS/EventManager.h>

#include <Gameplay/Components/Transform.h>
#include <Gameplay/Components/Rotation.h>
#include <Gameplay/Components/Billboard.h>

#include <Gameplay/Systems/RotationSystem.h>

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
			//m_Transform->SetLocalOrientation(0.0f, 90.0f, 90.0f);
		}

		void Update(EventManager& eventManager, float dt) override
		{
			glm::mat4 viewMatrix = Camera::ActiveCamera->GetViewMatrix();

			glm::mat4 matrix(1.0f);
			//matrix = glm::translate(matrix, m_Transform->getWorldPosition());
			matrix = glm::translate(matrix, m_Transform->GetWorldPosition());
			matrix[0] = glm::vec4(viewMatrix[0][0], viewMatrix[1][0], viewMatrix[2][0], 0.0f);
			matrix[1] = glm::vec4(viewMatrix[0][1], viewMatrix[1][1], viewMatrix[2][1], 0.0f);
			matrix[2] = glm::vec4(viewMatrix[0][2], viewMatrix[1][2], viewMatrix[2][2], 0.0f);
			//matrix = glm::rotate(matrix, glm::radians(90.0f), glm::vec3(.0f, 1.0f, .0f));
			//matrix = glm::rotate(matrix, glm::radians(90.ssz0f), glm::vec3(.0f, .0f, 1.0f));
			//matrix = glm::scale(matrix, m_Transform->GetLocalScale());
			//matrix = glm::rotate(matrix, glm::radians(180.0f), glm::vec3(1.0f, .0f, .0f));
			//matrix = glm::rotate(matrix, glm::radians(180.0f), glm::vec3(.0f, 1.0f, .0f));
			matrix = glm::rotate(matrix, glm::radians(-180.0f), glm::vec3(.0f, .0f, 1.0f));


			matrix = glm::scale(matrix, m_Transform->GetLocalScale());
			m_Transform->SetLocalMatrix(matrix);

		}
	};

}
