#pragma once
#include "pch.h"
#include "Engine.h"
namespace sixengine {
	class Color
	{
	public:
		glm::vec4 color;
		//statics
		static Color red;
		static Color green;
		static Color blue;
		static Color white;
		static Color black;
		static Color grey;

		Color(float r, float g, float b, float a);
		Color(glm::vec4 color);
	
		glm::vec4 operator* (Color* arg0);
		glm::vec4 operator* (glm::vec4 arg0);
		glm::vec4 operator* (Color arg0);
		friend std::ostream& operator<<(std::ostream& os, const Color c) {
			os << " Red: "<<c.color.r << " Green: " << c.color.g << " Blue: " << c.color.b << " Alpha: " << c.color.a;
			return os;
		}

	};

}

