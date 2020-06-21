#pragma once

#include "Core/CameraSystem/Camera.h"

namespace sixengine {

	class ShadowFrustum
	{
	private:
		const float m_Offset = 5.0f;
		const float m_ShadowDistance = 30.0f;

		float minX, maxX;
		float minY, maxY;
		float minZ, maxZ;

		float farHeight, farWidth, nearHeight, nearWidth;

		glm::mat4* m_LightView;
		GameObject* m_Center;
	public:
		ShadowFrustum(glm::mat4* lightView, GameObject* center);

		void Update();

		glm::vec4 CalculateFrustumCorner(glm::vec3 startPoint, glm::vec3 direction, float width);
		glm::vec4* CalculateFrustumVertices(glm::vec3& centerNear, glm::vec3& centerFar);

		glm::vec3 GetCenter();

		inline float GetWidth() const { return maxX - minX; }
		inline float GetHeight() const { return maxY - minY; }
		inline float GetLength() const { return maxZ - minZ; }
	};

}

