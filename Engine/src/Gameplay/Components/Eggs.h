#pragma once

#include "Core/ISerializable.h"
#include "Renderer/Model.h"

namespace sixengine
{
	class Eggs : ISerializable
	{
	public:

		Eggs(GameObject* go = nullptr) {}

		virtual void Load(std::iostream& stream) override {}
		virtual void Save(std::iostream& stream) override {}
	};
}