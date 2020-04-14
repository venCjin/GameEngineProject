#include "pch.h"
#include "Color.h"


namespace sixengine {

	const Color Color::RED = Color(1.0f, 0.0f, 0.0f, 1.0f);
	const Color Color::GREEN = Color(1.0f, 1.0f, 0.0f, 1.0f);
	const Color Color::BLUE = Color(0.0f, 0.0f, 1.0f, 1.0f);
	const Color Color::WHITE = Color(1.0f, 1.0f, 1.0f, 1.0f);
	const Color Color::BLACK = Color(0.0f, 0.0f, 0.0f, 1.0f);
	const Color Color::GREY = Color(.5f, .5f, .5f, 1.0f);


	Color::Color(float r, float g, float b, float a){
		value = glm::vec4(r, g, b, a);
	}

	Color::Color(glm::vec4 color)
	{
		this->value = color;
	}


	glm::vec4 Color::operator*(Color* arg0)
	{
		return this->value * arg0->value;
	}

	glm::vec4 Color::operator*(glm::vec4 arg0)
	{
		return this->value * arg0;
	}

	glm::vec4 Color::operator*(Color arg0)
	{
		return this->value * arg0.value;
	}

}