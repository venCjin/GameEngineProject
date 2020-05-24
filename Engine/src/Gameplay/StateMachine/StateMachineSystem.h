#pragma once

#include <ECS/SystemManager.h>

#include "State.h"

namespace sixengine
{
	SYSTEM(StateMachineSystem, State)
	{
		void Update(EventManager & eventManager, float dt) override
		{

		}
	};
}