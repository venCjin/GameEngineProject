#pragma once

#include "FollowCamera.h";

namespace sixengine
{
	class OrbitalCamera : public FollowCamera
	{
	public:
		float m_CurrentAngle = 0.0f;
		float m_Distance = 5.0f;
	};
}