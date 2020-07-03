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
			std::stringstream ss;
			if (m_AirText->ID == 0)
			{
				ss << "Health: " << int(m_AirText->player->m_Health);
				m_Text->text = ss.str();
			}
			else if (m_AirText->ID == 1)
			{
				ss << "Air: " << int(m_AirText->player->m_Air);
				m_Text->text = ss.str();
			}
		}
	};
}