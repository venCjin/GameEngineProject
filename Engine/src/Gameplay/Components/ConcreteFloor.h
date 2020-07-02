#pragma once

#include "Core/CameraSystem/Camera.h"
#include "Core/ISerializable.h"

namespace sixengine {

	struct ConcreteFloor : public ISerializable
	{
		ConcreteFloor(GameObject* go)
		{
		}

		virtual void Load(std::iostream& stream)
		{

		}

		virtual void Save(std::iostream& stream)
		{

		}
	};

}