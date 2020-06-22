#pragma once

#include "Core/ISerializable.h"
#include "Gameplay/Components/Mesh.h"
#include "Gameplay/GameObject.h"


namespace sixengine
{
	class Projectile : ISerializable
	{
		const float speed = 80.0f;

	public:
		Projectile(glm::vec3 dir, GameObject* go = nullptr)
			: speedDir(dir * speed) {}

		glm::vec3 speedDir;

		void SetDirection(glm::vec3 dir)
		{
			speedDir = dir * speed;
		}

		virtual void Load(std::iostream& stream) override {}
		virtual void Save(std::iostream& stream) override {}
	};
}