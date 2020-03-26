#pragma once

#include <Renderer/VertexArray.h>

namespace sixengine {

	struct TestMesh
	{
		VertexArray* VAO;

		TestMesh(VertexArray* vao) : VAO(vao) {}
	};

}