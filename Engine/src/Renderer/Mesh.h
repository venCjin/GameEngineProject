#pragma once
#include <glm/glm.hpp>

#include "Buffers.h"
namespace sixengine {

	class Mesh
	{
	private:
		unsigned int VBO, EBO;
		void SetupMesh();
		void SetupTexture(char* texturePath);
	public:
		unsigned int VAO;
		unsigned int texture;
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		void Draw(/*Shader shader*/);

		Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, char *texturePath);
		~Mesh();
	};
}
