#pragma once

#include <ECS/SystemManager.h>

#include <Gameplay/Components/Transform.h>
#include <Gameplay/Components/ScolopendraComponent.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "Core/CameraSystem/Camera.h"
#include <Physics\Components\DynamicBody.h>

namespace sixengine {

	SYSTEM(ScolopendraSystem, Transform, ScolopendraComponent)
	{
		
		void Update(EventManager & eventManager, float dt) override
		{
			

			glm::vec3 transformedPos = m_ScolopendraComponent->m_PlayerTransform->GetWorldPosition();
			transformedPos = glm::vec3(transformedPos.x, transformedPos.z, -transformedPos.y);

			m_ScolopendraComponent->m_Bones.begin()->second.m_Bone.SetLocalPosition(transformedPos);
			//m_ScolopendraComponent->m_Bones[0].m_Bone.SetLocalPosition(transformedPos);

			UpdateModelBones();			
		}

		void UpdateModelBones()
		{
			m_ScolopendraComponent->UpdateBones();
			std::map<int, Bone>::iterator it = m_ScolopendraComponent->m_Bones.begin();
			it->second.m_Bone.SetLocalOrientation((++it)->second.m_Bone.GetLocalOrientation());

			//m_ScolopendraComponent->m_Bones[0].m_Bone.SetLocalOrientation(m_ScolopendraComponent->m_Bones[1].m_Bone.GetLocalOrientation());

			for (int i = 0; i < m_ScolopendraComponent->m_Model->m_BoneInfo.size(); ++i)
			{
				if (m_ScolopendraComponent->m_Bones.find(i) != m_ScolopendraComponent->m_Bones.end())
					m_ScolopendraComponent->m_Model->m_BoneInfo[i].GlobalTransformation = m_ScolopendraComponent->m_Bones[i].m_Bone.GetLocalMatrix();
			}
		}
	};

}