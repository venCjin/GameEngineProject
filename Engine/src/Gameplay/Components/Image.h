#pragma once

#include <Renderer/Texture.h>

namespace sixengine {

	struct Image
	{
		Texture* texture;
		glm::vec2 size;

		Image(Texture* texture, glm::vec2 size)
			: texture(texture), size(size) {}
	};

}