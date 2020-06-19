#pragma once

#include <Gameplay/GameObject.h>

namespace sixengine
{
	class Collectable : ISerializable
	{
	public:
		Collectable(GameObject* go = nullptr) {}
		int hp = 10;

		virtual void Load(std::iostream& stream) override {}
		virtual void Save(std::iostream& stream) override {}
	};
}