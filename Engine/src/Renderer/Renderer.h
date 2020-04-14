#pragma once

#include "Renderer/VertexArray.h"
#include "Renderer/Shader.h"

namespace sixengine {

	class Renderer
	{
	public:
		Renderer();
		~Renderer();

		static void Render(const VertexArray* vertexArray, const Shader* shader);
		static void Clear(float r = 0.f, float g = 0.f, float b = 0.f);
	};

}

