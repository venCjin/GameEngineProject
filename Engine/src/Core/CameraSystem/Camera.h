#pragma once

#include <glm/glm.hpp>

#include "Engine.h"
#include "Gameplay/Components/Transform.h"
#include "Gameplay/GameObject.h"

namespace sixengine 
{
	class Camera
	{
	public:
		inline static Camera* ActiveCamera = nullptr;

		GameObject* m_GameObject = nullptr;
		Transform* m_Transform = nullptr;

		glm::mat4 m_ProjectionMatrix = glm::mat4(1.0f);

		float m_Zoom = 45.0f;
		float m_NearPlane = 0.01f;
		float m_FarPlane = 1000.0f;

	public:
		Camera(GameObject* go)
		{
			m_GameObject = go;
			m_Transform = m_GameObject->GetComponent<Transform>().Get();

			if (Camera::ActiveCamera == NULL)
			{
				Camera::ActiveCamera = this;
			}
		}

		void SetPerspective(float aspectRatio)
		{
			m_ProjectionMatrix = glm::perspective(glm::radians(m_Zoom), aspectRatio, m_NearPlane, m_FarPlane);
		}

		void SetOrthogonal(float width, float height)
		{
			m_ProjectionMatrix = glm::ortho(0.f, width, 0.f, height, m_NearPlane, m_FarPlane);
		}

		inline const glm::mat4& GetProjectionMatrix()
		{
			return m_ProjectionMatrix;
		}

		glm::mat4 GetViewMatrix()
		{
			glm::vec3 position = m_Transform->GetWorldPosition();
			glm::vec3 forward = m_Transform->GetForward();
			glm::vec3 upwards = m_Transform->GetUpwards();

			return glm::lookAt(position, position + forward, upwards);
		}
	};
}
