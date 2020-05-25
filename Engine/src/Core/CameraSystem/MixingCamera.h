#pragma once

#include <glm/glm.hpp>

#include "Camera.h"
#include "Engine.h"
#include "Gameplay/Components/Transform.h"
#include "Gameplay/GameObject.h"

namespace sixengine
{
	class MixingCamera
	{
	private:
		Camera* m_ActiveCamera;
	public:
		Camera* m_TargetCamera;

		MixingCamera(GameObject* go)
		{
			m_ActiveCamera = go->GetComponent<Camera>().Get();
		}

		void SetTargetCamera(Camera* camera)
		{
			m_TargetCamera = camera;
		}

		friend class MixingCameraSystem;
	};
}
