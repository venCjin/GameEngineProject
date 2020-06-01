#include "pch.h"
#include "ShadowFrustum.h"

namespace sixengine {

	ShadowFrustum::ShadowFrustum(glm::mat4* lightView, GameObject* center)
		: m_LightView(lightView), m_Center(center)
	{
		float aspectRation = Application::Get().GetWindow().GetWidth() / Application::Get().GetWindow().GetHeight();

		farWidth = m_ShadowDistance * glm::tan(glm::radians(Camera::ActiveCamera->m_Zoom));
		nearWidth = Camera::ActiveCamera->m_NearPlane * glm::tan(glm::radians(Camera::ActiveCamera->m_Zoom));

		farHeight = farWidth / aspectRation;
		nearHeight = nearWidth / aspectRation;
	}

	void ShadowFrustum::Update()
	{
		glm::vec3 forwardVector = Camera::ActiveCamera->m_Transform->GetForward(); //m_Center->GetComponent<Transform>()->GetForward(); 

		glm::vec3 toNear = forwardVector * Camera::ActiveCamera->m_NearPlane;
		glm::vec3 toFar = forwardVector * m_ShadowDistance;

		glm::vec3 centerNear = toNear + Camera::ActiveCamera->m_Transform->GetWorldPosition(); //m_Center->GetComponent<Transform>()->GetWorldPosition();
		glm::vec3 centerFar = toFar + Camera::ActiveCamera->m_Transform->GetWorldPosition(); //m_Center->GetComponent<Transform>()->GetWorldPosition();

		glm::vec4* points = CalculateFrustumVertices(centerNear, centerFar);

		minX = points[0].x;
		maxX = points[0].x;
		minY = points[0].y;
		maxY = points[0].y;
		minZ = points[0].z;
		maxZ = points[0].z;

		glm::vec4 point;
		for (int i = 1; i < 8; i++)
		{
			point = points[i];

			if (point.x > maxX)
				maxX = point.x;
			else if (point.x < minX)
				minX = point.x;

			if (point.y > maxY)
				maxY = point.y;
			else if (point.y < minY)
				minY = point.y;

			if (point.z > maxZ)
				maxZ = point.z;
			else if (point.z < minZ)
				minZ = point.z;
		}

		maxZ += m_Offset;

		delete points;
	}

	glm::vec4 ShadowFrustum::CalculateFrustumCorner(glm::vec3 startPoint, glm::vec3 direction, float width)
	{
		glm::vec4 point = glm::vec4(startPoint + glm::vec3(direction.x * width, direction.y * width, direction.z * width), 1.0f);
		point = *m_LightView * point;

		return point;
	}

	glm::vec4* ShadowFrustum::CalculateFrustumVertices(glm::vec3& centerNear, glm::vec3& centerFar)
	{
		glm::vec3 upVector = Camera::ActiveCamera->m_Transform->GetUpwards(); //m_Center->GetComponent<Transform>()->GetUpwards();
		glm::vec3 rightVector = Camera::ActiveCamera->m_Transform->GetRight(); //m_Center->GetComponent<Transform>()->GetRight();
		glm::vec3 downVector = -upVector;
		glm::vec3 leftVector = -rightVector;

		glm::vec3 farTop = centerFar + glm::vec3(upVector.x * farHeight, upVector.y * farHeight, upVector.z * farHeight);
		glm::vec3 farBottom = centerFar + glm::vec3(downVector.x * farHeight, downVector.y * farHeight, downVector.z * farHeight);
		glm::vec3 nearTop = centerNear + glm::vec3(upVector.x * nearHeight, upVector.y * nearHeight, upVector.z * nearHeight);
		glm::vec3 nearBottom = centerNear + glm::vec3(downVector.x * nearHeight, downVector.y * nearHeight, downVector.z * nearHeight);

		glm::vec4* points = new glm::vec4[8];
		points[0] = CalculateFrustumCorner(farTop, rightVector, farWidth);
		points[1] = CalculateFrustumCorner(farTop, leftVector, farWidth);
		points[2] = CalculateFrustumCorner(farBottom, rightVector, farWidth);
		points[3] = CalculateFrustumCorner(farBottom, leftVector, farWidth);
		points[4] = CalculateFrustumCorner(nearTop, rightVector, nearWidth);
		points[5] = CalculateFrustumCorner(nearTop, leftVector, nearWidth);
		points[6] = CalculateFrustumCorner(nearBottom, rightVector, nearWidth);
		points[7] = CalculateFrustumCorner(nearBottom, leftVector, nearWidth);

		return points;
	}

	glm::vec3 ShadowFrustum::GetCenter()
	{
		float x = (minX + maxX) / 2.0f;
		float y = (minY + maxY) / 2.0f;
		float z = (minZ + maxZ) / 2.0f;

		glm::vec4 cen(x, y, z, 1.0f);
		glm::mat4 invertedLight = glm::inverse(*m_LightView);
		return invertedLight * cen;
	}

}