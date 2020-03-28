#pragma once
#include "pch.h"
#include "Engine.h"
namespace sixengine {
	class Color
	{
	public:
		glm::vec4 value;
		//statics
		static const Color RED;
		static const Color GREEN;
		static const Color BLUE;
		static const Color WHITE;
		static const Color BLACK;
		static const Color GREY;

		Color(float r, float g, float b, float a);
		Color(glm::vec4 color);
	
		glm::vec4 operator* (Color* arg0);
		glm::vec4 operator* (glm::vec4 arg0);
		glm::vec4 operator* (Color arg0);
		std::string  ToString() {
			std::stringstream stringStream;
			stringStream << " Red: "<< value.r << " Green: " << value.g << " Blue: " << value.b << " Alpha: " << value.a;
			return stringStream.str();
		}

	};

}

