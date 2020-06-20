#pragma once

#include "Core/ISerializable.h"
#include "Gameplay/Components/Generator.h"

namespace sixengine
{
	class Gate : ISerializable
	{
	public:
		Gate(GameObject* = nullptr)
			: closed(true), openingSpeed(glm::vec3(0.0f, -2.0f, 0.0f)) {}

		bool closed;
		std::vector<Generator*> generators;

		float openingDuration = 3.0f;
		glm::vec3 openingSpeed;

		void AddGenerator(Generator* gen)
		{
			generators.push_back(gen);
		}

		virtual void Load(std::iostream& stream) override {}
		virtual void Save(std::iostream& stream) override {}
	};
}