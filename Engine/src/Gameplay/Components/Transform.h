#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Gameplay/GameObject.h"

#include <algorithm>

namespace sixengine {

	class Transform
	{
	private:
		Transform* m_Parent;
		std::vector<Transform*> m_Children;

		glm::mat4 m_Local;

	public:
		Transform(GameObject* parent) : m_Local(glm::mat4(1.0f)) {}

		// Sets the parent of the transform.
		void SetParent(Transform* parent)
		{
			// Remove this from parent't children list
			if (m_Parent)
			{
				m_Parent->m_Children.erase(std::find(m_Parent->m_Children.begin(), m_Parent->m_Children.end(), this));
			}

			m_Parent = parent;
			m_Parent->m_Children.push_back(this);
		}

		// Retruns the parent of the transform.
		Transform* GetParent()
		{
			return m_Parent;
		}

		// Returns the world space position of the Transform.
		glm::vec3 GetWorldPosition()
		{
			if (m_Parent)
			{
				return m_Parent->getWorldPosition() + GetLocalPosition();
			}
			else
			{
				return GetLocalPosition();
			}
		}

		glm::vec3 getWorldPosition()
		{
			return GetWorldPosition();
		}

		// Sets the world space position of the Transform.
		void SetWorldPosition(glm::vec3 position)
		{
			if (m_Parent)
			{
				SetLocalPosition(position - m_Parent->getWorldPosition());
			}
			else
			{
				SetLocalPosition(position);
			}
		}

		// Sets the world space position of the Transform.
		void SetWorldPosition(float x, float y, float z)
		{
			SetWorldPosition(glm::vec3(x, y, z));
		}

		// Returns position of the transform relative to the parent transform.
		glm::vec3 GetLocalPosition()
		{
			return glm::vec3(m_Local[3]);
		}

		// Sets position of the transform relative to the parent transform.
		void SetLocalPosition(glm::vec3 position)
		{
			m_Local[3][0] = position.x;
			m_Local[3][1] = position.y;
			m_Local[3][2] = position.z;
		}

		// Sets position of the transform relative to the parent transform.
		void SetLocalPosition(float x, float y, float z)
		{
			m_Local[3][0] = x;
			m_Local[3][1] = y;
			m_Local[3][2] = z;
		}

		// Moves the transform in the directionand distance of translation.
		void Translate(glm::vec3 translation)
		{
			m_Local[3][0] += translation.x;
			m_Local[3][1] += translation.y;
			m_Local[3][2] += translation.z;
		}

		// Moves the transform in the directionand distance of translation.
		void Translate(float x, float y, float z)
		{
			Translate(glm::vec3(x, y, z));
		}


		// Returs a Quaternion that stores the rotation of the Transform in world space.
		glm::quat GetWorldRotation()
		{
			if (m_Parent)
			{
				return GetLocalRotation() * m_Parent->GetWorldRotation();
			}
			else
			{
				return GetLocalRotation();
			}
		}

		// Sets the rotation of the Transform in local space.
		void SetWorldRotation(glm::quat rotation)
		{
			if (m_Parent)
			{
				SetLocalRotation(rotation * glm::inverse(m_Parent->GetWorldRotation()));
			}
			else
			{
				SetLocalRotation(rotation);
			}
		}

		// Returs a Quaternion that stores the rotation of the Transform in local space.
		glm::quat GetLocalRotation()
		{
			glm::mat3 rotation = glm::mat3(m_Local);
			glm::vec3 scale = GetLocalScale();

			rotation[0] /= scale.x;
			rotation[1] /= scale.y;
			rotation[2] /= scale.z;

			return glm::quat_cast(rotation);
		}

		// Sets the rotation of the Transform in local space.
		void SetLocalRotation(glm::quat rotation)
		{
			glm::mat4 rot = glm::toMat4(rotation);
			glm::vec3 scale = GetLocalScale();

			m_Local[0] = rot[0] * scale.x;
			m_Local[1] = rot[1] * scale.y;
			m_Local[2] = rot[2] * scale.z;
		}

		// Rotates the transform by given quaternion.
		void Rotate(glm::quat rotation)
		{
			m_Local = m_Local * glm::toMat4(rotation);
		}


		// The rotation as Yaw, Pitch and Roll angles in degrees.
		glm::vec3 GetWorldOrientation()
		{
			glm::quat rotation = GetWorldRotation();
			glm::vec3 orientation = glm::vec3();

			orientation.x = glm::yaw(rotation);
			orientation.y = glm::pitch(rotation);
			orientation.z = glm::roll(rotation);

			orientation.x = glm::degrees(orientation.x);
			orientation.y = glm::degrees(orientation.y);
			orientation.z = glm::degrees(orientation.z);

			return orientation;
		}

		// The rotation as Yaw, Pitch and Roll angles in degrees relative to the parent transform's rotation.
		glm::vec3 GetLocalOrientation()
		{
			glm::quat rotation = GetWorldRotation();
			glm::vec3 orientation = glm::vec3();

			orientation.x = glm::yaw(GetLocalRotation());
			orientation.y = glm::pitch(GetLocalRotation());
			orientation.z = glm::roll(GetLocalRotation());

			orientation.x = glm::degrees(orientation.x);
			orientation.y = glm::degrees(orientation.y);
			orientation.z = glm::degrees(orientation.z);

			return orientation;
		}

		// Sets the rotation as Yaw, Pitch and Roll angles in degrees relative to the parent transform's rotation.
		void SetLocalOrientation(glm::vec3 euler)
		{
			euler.x = glm::radians(euler.x);
			euler.y = glm::radians(euler.y);
			euler.z = glm::radians(euler.z);

			glm::mat4 rotation = glm::yawPitchRoll(euler.x, euler.y, euler.z);
			glm::vec3 scale = GetWorldScale();

			m_Local[0] = rotation[0] * scale.x;
			m_Local[1] = rotation[1] * scale.y;
			m_Local[2] = rotation[2] * scale.z;
		}

		// Sets the rotation as Yaw, Pitch and Roll angles in degrees relative to the parent transform's rotation.
		void SetLocalOrientation(float x, float y, float z)
		{
			SetLocalOrientation(glm::vec3(x, y, z));
		}

		// Rotates the transform by given Yaw, Pitch and Roll angles.
		void Rotate(glm::vec3 rotation)
		{
			Rotate(rotation.x, rotation.y, rotation.z);
		}

		// Rotates the transform by given Yaw, Pitch and Roll angles.
		void Rotate(float x, float y, float z)
		{
			m_Local *= glm::yawPitchRoll(x, y, z);
		}


		// Returns the global scale of the object.
		glm::vec3 GetWorldScale()
		{
			if (m_Parent)
			{
				return m_Parent->GetWorldScale() * GetLocalScale();
			}
			else
			{
				return GetLocalScale();
			}
		}

		// Returns the scale of the transform relative to the parent.
		glm::vec3 GetLocalScale()
		{
			float x = glm::length(m_Local[0]);
			float y = glm::length(m_Local[1]);
			float z = glm::length(m_Local[2]);

			return glm::vec3(x, y, z);
		}

		// Sets the scale of the transform relative to the parent.
		void SetLocalScale(glm::vec3 localScale)
		{
			auto current = GetLocalScale();

			m_Local[0] *= localScale.x / current.x;
			m_Local[1] *= localScale.y / current.y;
			m_Local[2] *= localScale.z / current.z;
		}

		// Sets the scale of the transform relative to the parent.
		void SetLocalScale(float x, float y, float z)
		{
			auto current = GetLocalScale();

			m_Local[0] *= x / current.x;
			m_Local[1] *= y / current.y;
			m_Local[2] *= z / current.z;
		}


		// Returns the forward vector from this Transform, in world space.
		glm::vec3 GetForward()
		{
			return (GetWorldRotation() * glm::vec3(0.0f, 0.0f, -1.0f));
		}

		// Returns the right vector from this Transform, in world space.
		glm::vec3 GetRight()
		{
			return (GetWorldRotation() * glm::vec3(1.0f, 0.0f, 0.0f));
		}

		// Returns the up vector from this Transform, in world space.
		glm::vec3 GetUpwards()
		{
			return (GetWorldRotation() * glm::vec3(0.0f, 1.0f, 0.0f));
		}


		// Rotates the transform so the forward vector points at target's current position.
		void LookAt(Transform* target)
		{
			if (target == nullptr) { return; }

			glm::vec3 forward = target->getWorldPosition() - this->getWorldPosition();
			forward = glm::normalize(forward);

			glm::vec3 right = glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f));
			glm::vec3 upwards = glm::cross(right, forward);

			m_Local = glm::inverse((glm::lookAt(getWorldPosition(), getWorldPosition() + forward, upwards)));
		}


		// Transforms position from local space to world space.
		glm::vec3 TransformPoint(glm::vec3 point)
		{
			return m_Local * glm::vec4(point, 1.0f);
		}

		// Transforms position from world space to local space.
		glm::vec3 InverseTransformPoint(glm::vec3 point)
		{
			return glm::inverse(m_Local) * glm::vec4(point, 1.0f);
		}


		glm::mat4 GetModelMatrix()
		{
			if (m_Parent)
			{
				return m_Parent->GetModelMatrix() * m_Local;
			}
			else
			{
				return m_Local;
			}
		}

		glm::mat4 GetLocalMatrix()
		{
			return m_Local;
		}

		void SetLocalMatrix(glm::mat4 model)
		{
			m_Local = model;
		}


		void SetWorld(glm::mat4 world)
		{
			m_Local = world;
		}

		const glm::mat4& GetWorld()
		{
			return m_Local;
		}
	};

}

