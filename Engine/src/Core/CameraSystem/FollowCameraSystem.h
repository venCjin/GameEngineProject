#pragma once

#include <ECS/SystemManager.h>

#include "Core/CameraSystem/FollowCamera.h"
#include <Gameplay/Components/Transform.h>

namespace sixengine 
{
	SYSTEM(FollowCameraSystem, FollowCamera, Transform)
	{
		void Update(EventManager & eventManager, float dt) override
		{
			if (m_FollowCamera->m_LookTarget)
			{
				m_Transform->LookAt(m_FollowCamera->m_LookTarget);
			}

			if (m_FollowCamera->m_FollowTarget)
			{
				auto current = m_Transform->getWorldPosition();
				auto target = m_FollowCamera->m_FollowTarget->getWorldPosition() + m_FollowCamera->m_FollowOffset;
				
				auto smooth = 0.75f;
				auto position = target * (1.0f - smooth) + current * smooth;

				m_Transform->SetWorldPosition(position);
			}
		}
	};
}
