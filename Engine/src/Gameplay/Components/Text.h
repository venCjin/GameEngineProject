#pragma once

#include <string>

namespace sixengine {

	struct Text
	{
		std::string text;
		glm::vec3 color;
		float size;

		Text(std::string text, glm::vec3 color, float size)
			: text(text), color(color), size(size) {}

		void SetText(std::string newText)
		{
			text = newText;
		}
	};

}