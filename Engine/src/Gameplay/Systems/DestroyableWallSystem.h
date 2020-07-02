#pragma once

#include <ECS/SystemManager.h>
#include <ECS/EventManager.h>

#include <Gameplay/Components/DestroyableWall.h>
#include <Gameplay/Components/Transform.h>

namespace sixengine {

	SYSTEM(DestroyableWallSystem, DestroyableWall)
	{
	public:
		void OnStart(EventManager & eventManager) override
		{

		}

		void Update(EventManager & eventManager, float dt) override
		{
			if (m_DestroyableWall->m_Destroyed)
			{
				for (auto child : m_DestroyableWall->m_GameObject->GetComponent<Transform>()->GetChildren())
				{
					auto pos = child->GetWorldPosition();
					
					if (pos.y <= 0.0f) { continue; }
					
					auto v = glm::normalize(child->GetLocalPosition()) * (float)Timer::Instance()->DeltaTime() * 0.75f;
					v.y = 0.0f;

					pos += v;
					pos.y = max(0.0f, pos.y - Timer::Instance()->DeltaTime() * 5.0f);
					child->SetWorldPosition(pos);


				}
			}
		}
	};

}
