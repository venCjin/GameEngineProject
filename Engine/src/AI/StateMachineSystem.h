#pragma once
#pragma once

#include <ECS/SystemManager.h>
#include <ECS/EventManager.h>

#include "AI/StateMachine.h"

namespace sixengine {

	SYSTEM(StateMachineSystem, StateMachine)
	{
	public:
		void Update(EventManager& eventManager, float dt) override
		{
			m_StateMachine->OnUpdate();
		}
	};

}
