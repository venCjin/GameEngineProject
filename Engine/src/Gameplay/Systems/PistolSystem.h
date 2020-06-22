#pragma once

#include <ECS/SystemManager.h>

#include <Gameplay/Components/Pistol.h>
#include <Gameplay/Components/Transform.h>

namespace sixengine {

	SYSTEM(PistolSystem, Pistol, Transform)
	{
		void Update(EventManager & eventManager, float dt) override
		{
			m_Transform->GetParent()->SetWorld(m_Pistol->m_OwnerTransform->GetModelMatrix() * m_Pistol->m_Model->GetBoneInfo(m_Pistol->m_BoneInfoIndex)->FinalTransformation);
		}
	};
}