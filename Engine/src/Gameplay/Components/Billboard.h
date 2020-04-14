#pragma once

#include "Core/Camera.h"

namespace sixengine {

	struct Billboard
	{
		Camera* m_Camera;

		Billboard(Camera* camera) : m_Camera(camera){}
	};

}