#pragma once
#include <sstream>
#include <ECS/SystemManager.h>
#include <Gameplay/Components/AirText.h>
#include <Gameplay\Components\SimplePlayer.h>
#include <Gameplay\Components\Text.h>


namespace sixengine
{
	SYSTEM(AirTextSystem, Text, AirText)
	{
	public:
		void Update(EventManager& eventManager, float dt) override
		{
			return;

			std::stringstream ss;
			ss << "Air:" << m_AirText->player->air;
			m_Text->text = ss.str();
		}
	};
}