#pragma once

#include "Core/ISerializable.h"
#include "Gameplay/Components/Mesh.h"
#include "Gameplay/GameObject.h"


namespace sixengine
{
	class Projectile : ISerializable
	{
		const float speed = 10.0f;
	public:
		Projectile(glm::vec3 dir, unsigned int timeToLive = 2.0f, GameObject* go = nullptr)
			: speedDir(dir * speed), endTime(Timer::Instance()->GetTime(SECOND) + timeToLive) {}

		glm::vec3 speedDir;
		unsigned int endTime;

		virtual void Load(std::iostream& stream) override {}
		virtual void Save(std::iostream& stream) override {}
	};
}