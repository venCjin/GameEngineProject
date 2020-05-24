#pragma once

#include "Core/CameraSystem/Camera.h"

namespace sixengine {

	struct Billboard
	{
		Camera* m_Camera;

		Billboard(Camera* camera)
		{
			m_Camera = camera;
		}
	};

}