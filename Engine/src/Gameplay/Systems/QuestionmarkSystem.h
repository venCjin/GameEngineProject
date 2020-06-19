#pragma once
#include <ECS/SystemManager.h>
#include <Gameplay/Components/Questionmark.h>
#include <Gameplay/Components/Transform.h>


namespace sixengine
{
	SYSTEM(QuestionmarkSystem, Questionmark, Transform)
	{
		void Update(EventManager & eventManager, float dt) override
		{
			GameObject* enemy = m_Questionmark->enemy;
			m_Transform->SetLocalPosition(enemy->GetComponent<Transform>()->getWorldPosition() + glm::vec3(0.0f, 2.0f, 0.0f));
			m_Questionmark->fill = enemy->GetComponent<Enemy>()->GetDetectionLevel();
			if (enemy->GetComponent<Enemy>()->GetDetectionLevel() > 0.1f) m_Questionmark->visable = 1;
			else m_Questionmark->visable = 0;

		}
	};
}

