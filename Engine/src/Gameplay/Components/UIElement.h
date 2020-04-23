#pragma once
#include <iostream>
#include <Engine.h>
namespace sixengine {
	class UIElement
	{
	public:
		glm::vec2 m_Position; 
		float m_Width, m_Height; //in pixels
		UIElement(glm::vec2 position, float width, float height) : m_Position(position), m_Width(width), m_Height(height){}
	};
}
