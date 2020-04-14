#pragma once

#include "Renderer/VertexArray.h"

namespace sixengine {

	struct TestMesh
	{
		VertexArray* VAO;

		TestMesh(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices)
		{
			VertexBuffer* vbo = new VertexBuffer(vertices.data(), vertices.size());
			vbo->SetLayout({
				{ VertexDataType::VEC3F, "position" },
				{ VertexDataType::VEC3F, "normal" },
				{ VertexDataType::VEC2F, "texCoords" },
				{ VertexDataType::VEC4I, "boneIDs" },
				{ VertexDataType::VEC4F, "boneWeights" }
			});

			IndexBuffer* ibo = new IndexBuffer(indices.data(), indices.size());

			VAO = new VertexArray();
			VAO->AddVertexBuffer(*vbo);
			VAO->AddIndexBuffer(*ibo);
		}
	};

}