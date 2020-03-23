#pragma once

#include "Renderer\VertexArray.h"

namespace sixengine {

	class Renderer
	{
	public:
		VertexArray* m_VAO = nullptr;

		Renderer();
		~Renderer();

		void Init();
		void Render();
	};

}

