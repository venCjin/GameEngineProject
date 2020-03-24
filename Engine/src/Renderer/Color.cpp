#include "pch.h"
#include "Color.h"


namespace sixengine
{
	Color Color::red = Color(1.0f, 0.0f, 0.0f, 1.0f);
	Color Color::green = Color(1.0f, 1.0f, 0.0f, 1.0f);
	Color Color::blue = Color(0.0f, 0.0f, 1.0f, 1.0f);
	Color Color::white = Color(1.0f, 1.0f, 1.0f, 1.0f);
	Color Color::black = Color(0.0f, 0.0f, 0.0f, 1.0f);
	Color Color::grey = Color(.5f, .5f, .5f, 1.0f);


	Color::Color(float r, float g, float b, float a){
		color = glm::vec4(r, g, b, a);
	}

	Color::Color(glm::vec4 color)
	{
		this->color = color;
	}


	glm::vec4 Color::operator*(Color* arg0)
	{
		return this->color * arg0->color;
	}

	glm::vec4 Color::operator*(glm::vec4 arg0)
	{
		return this->color * arg0;
	}

	glm::vec4 Color::operator*(Color arg0)
	{
		return this->color * arg0.color;
	}

}