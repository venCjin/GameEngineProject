#pragma once
#include <iostream>
#include <Engine.h>
namespace sixengine {
	class UIRoot
	{
	public:
		//Texture m_Texture;
		//glm::vec2 m_Position;
	public:
		Camera* m_Camera;
		UIRoot(Camera* camera) : m_Camera(camera){}
	};
}
