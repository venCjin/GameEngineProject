#include "pch.h"
#include "Mesh.h"
#include <glad/glad.h> 
#include "stb_image.h"

namespace sixengine {

	void Mesh::SetupMesh()
	{

		VertexBuffer* vbo = new VertexBuffer(m_Vertices.data(), m_Vertices.size() * sizeof(Vertex));
		vbo->SetLayout({
			{ VertexDataType::VEC3F, "position" },
			{ VertexDataType::VEC3F, "normal" },
			{ VertexDataType::VEC2F, "texcoord" },
			});

		
		IndexBuffer* ibo = new IndexBuffer(m_Indices.data(), m_Indices.size());

		VAO = new VertexArray();
		VAO->AddVertexBuffer(*vbo);
		VAO->AddIndexBuffer(*ibo);
		/*glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

		glBindVertexArray(0);*/
	}	

	void Mesh::Draw(Shader* shader)
	{		
		unsigned int diffuseNr = 1;
		unsigned int specularNr = 1;
		unsigned int normalNr = 1;
		unsigned int heightNr = 1;
		for (unsigned int i = 0; i < m_Textures.size(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
			// retrieve texture number (the N in diffuse_textureN)
			std::string number;
			std::string name = m_Textures[i].type;
			if (name == "texture_diffuse")
				number = std::to_string(diffuseNr++);
			else if (name == "texture_specular")
				number = std::to_string(specularNr++); // transfer unsigned int to stream
			else if (name == "texture_normal")
				number = std::to_string(normalNr++); // transfer unsigned int to stream
			else if (name == "texture_height")
				number = std::to_string(heightNr++); // transfer unsigned int to stream

			// now set the sampler to the correct texture unit
			glUniform1i(glGetUniformLocation(shader->m_ID, (name + number).c_str()), i);
			// and finally bind the texture
			glBindTexture(GL_TEXTURE_2D, m_Textures[i].id);
		}
		
		VAO->Bind();

		glDrawElements(GL_TRIANGLES, VAO->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, 0);
		//glBindVertexArray(0);

	}

	Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
	{
		this->m_Vertices = vertices;
		this->m_Indices = indices;
		this->m_Textures = textures;

		SetupMesh();
	}

	Mesh::~Mesh()
	{
	}
}
