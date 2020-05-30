#pragma once

#include "Core/CameraSystem/Camera.h"
#include "Core/ISerializable.h"

namespace sixengine {

	struct Billboard : public ISerializable
	{
		Camera* m_Camera;

		Billboard(GameObject* go)
		{
			m_Camera = Camera::ActiveCamera;
		}

        virtual void Load(std::iostream& stream)
        {
        }
        virtual void Save(std::iostream& stream)
        {
            //TODO:
        }
	};

}