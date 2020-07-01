#pragma once

#include <Core/ISerializable.h>

namespace sixengine
{
	class UndergroundObject : ISerializable
	{
	public:
		UndergroundObject(GameObject* go = nullptr) {}

		virtual void Load(std::iostream& stream) override {}
		virtual void Save(std::iostream& stream) override {}
	};
}