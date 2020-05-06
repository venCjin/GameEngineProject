#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

#include "Core/Application.h"
#include "Core/Input.h"
#include "Core/Timer.h"

#include "Gameplay/GameObject.h"
#include "Gameplay/Components/Transform.h"
#include "ECS/EntityManager.h"

#include "Timer.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>

namespace sixengine {

	// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
	enum class CameraMovement
	{
		FORWARD,
		BACKWARD,
		LEFT,
		RIGHT,
		UP,
		DOWN
	};

	// Default camera values
	const float YAW = -90.0f;
	const float PITCH = 0.0f;
	const float SPEED = 10.0f;
	const float SENSITIVITY = 0.1f;
	const float ZOOM = 45.0f;


	// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
	class Camera
	{
	private:
		float m_CurrentAngle = 0.0f;
		glm::vec3 m_OffsetFromCharacter = glm::vec3(0.0f, 4.0f, 6.0f);

	public:
		class ComponentHandle<Transform> m_Target;

		// Camera Attributes
		glm::mat4 m_ProjectionMatrix;
		glm::vec3 m_Position;
		glm::vec3 m_Front;
		glm::vec3 m_Up;
		glm::vec3 m_Right;
		glm::vec3 m_WorldUp;
	private:
		// Euler Angles
		float m_Yaw;
		float m_Pitch;
		// Camera options
		float m_MovementSpeed;
		float m_MouseSensitivity;
		float m_Zoom;
		float m_NearPlane = 0.01f;
		float m_FarPlane = 1000.0f;
	public:
		// Constructor with vectors
		Camera(glm::vec3 position = glm::vec3(0.0f, 1.0f, 2.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) 
			: m_Front(glm::vec3(0.0f, 0.0f, -1.0f)), m_MovementSpeed(SPEED), m_MouseSensitivity(SENSITIVITY), m_Zoom(ZOOM)
		{
			m_Position = position;
			m_WorldUp = up;
			m_Yaw = yaw;
			m_Pitch = pitch;
			updateCameraVectors();
			//flyCameraInit();
		}
		// Constructor with scalar values
		Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) 
			: m_Front(glm::vec3(0.0f, 0.0f, -1.0f)), m_MovementSpeed(SPEED), m_MouseSensitivity(SENSITIVITY), m_Zoom(ZOOM)
		{
			m_Position = glm::vec3(posX, posY, posZ);
			m_WorldUp = glm::vec3(upX, upY, upZ);
			m_Yaw = yaw;
			m_Pitch = pitch;
			updateCameraVectors();
			//flyCameraInit();
		}

		void MakePerspective(float aspectRatio)
		{
			flyCameraInit();
			SetProjectionMatrix(glm::perspective(glm::radians(m_Zoom), aspectRatio, m_NearPlane, m_FarPlane));
		}

		void MakeOrtho(float width, float height)
		{
			SetProjectionMatrix(glm::ortho(0.f, width, 0.f, height, m_NearPlane, m_FarPlane));
		}

		void SetProjectionMatrix(glm::mat4 proj)
		{
			m_ProjectionMatrix = proj;
		}

		inline const glm::mat4& GetProjectionMatrix()
		{
			return m_ProjectionMatrix;
		}

		// Returns the view matrix calculated using Euler Angles and the LookAt Matrix
		glm::mat4 GetViewMatrix()
		{
			return glm::lookAt(m_Position, m_Position + m_Front, m_Up);
		}

		// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
		void ProcessKeyboard(CameraMovement direction, float deltaTime)
		{
			float velocity = m_MovementSpeed * deltaTime;
			if (direction == CameraMovement::FORWARD)
				m_Position += m_Front * velocity;
			if (direction == CameraMovement::BACKWARD)
				m_Position -= m_Front * velocity;
			if (direction == CameraMovement::LEFT)
				m_Position -= m_Right * velocity;
			if (direction == CameraMovement::RIGHT)
				m_Position += m_Right * velocity;
			if (direction == CameraMovement::UP)
				m_Position += m_Up * velocity;
			if (direction == CameraMovement::DOWN)
				m_Position -= m_Up * velocity;
		}

		// Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
		void ProcessMouseScroll(float yoffset)
		{
			if (m_Zoom >= 1.0f && m_Zoom <= 45.0f)
				m_Zoom -= yoffset;
			if (m_Zoom <= 1.0f)
				m_Zoom = 1.0f;
			if (m_Zoom >= 45.0f)
				m_Zoom = 45.0f;
		}
		glm::vec3 GetPosition() { return m_Position; }
		glm::vec3 GetUp() { return m_Up; }
		float GetYaw() { return m_Yaw; }
		float GetPitch() { return m_Pitch; }
	private:
		// Calculates the front vector from the Camera's (updated) Euler Angles
		void updateCameraVectors()
		{
			//// Calculate the new Front vector
			//glm::vec3 front;
			//front.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
			//front.y = sin(glm::radians(m_Pitch));
			//front.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
			//m_Front = glm::normalize(front);
			//// Also re-calculate the Right and Up vector
			//m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
			//m_Up = glm::normalize(glm::cross(m_Right, m_Front));
		}

		// Camera movement
		void flyCameraInit()
		{
			Input::AddKeyPressedBinding(KeyCode::LEFT_CONTROL, &Camera::toogleCursor, this);
			Input::AddMouseDeltaPosBinding(&Camera::mouseMove, this);
		}

		void toogleCursor() 
		{ 
			Application::Get().GetWindow().SwitchCursorVisibility(); 
		}



		void mouseMove(float xoffset, float yoffset) 
		{ 
			m_CurrentAngle -= xoffset * m_MouseSensitivity;

			//xoffset *= m_MouseSensitivity;
			//yoffset *= m_MouseSensitivity;

			//m_Yaw += xoffset;
			//m_Pitch += yoffset;

			//// Make sure that when pitch is out of bounds, screen doesn't get flipped
			//if (m_Pitch > 89.0f)
			//	m_Pitch = 89.0f;
			//if (m_Pitch < -89.0f)
			//	m_Pitch = -89.0f;

			//// Update Front, Right and Up Vectors using the updated Euler angles
			//updateCameraVectors();
		}

		public:
		void OnUpdate(float dt)
		{
			m_Position = m_Target->getWorldPosition();
			
			glm::vec3 offset = glm::rotate(m_OffsetFromCharacter, glm::radians(m_CurrentAngle), glm::vec3(0.0f, 1.0f, 0.0f));
			m_Position += offset;

			m_Front = glm::normalize(m_Target->getWorldPosition() - m_Position);
			m_Right = glm::cross(m_Front, glm::vec3(0.0f, 1.0f, 0.0f));
			m_Up = glm::cross(m_Right, m_Front);
		}
	};

}
