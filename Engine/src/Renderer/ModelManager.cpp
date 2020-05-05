#include "pch.h"
#include "ModelManager.h"

namespace sixengine {

	void ModelManager::AddModel(std::string path)
	{
		if (!m_ModelsAlreadyLoaded && m_ModelsMapping.find(path) == m_ModelsMapping.end())
		{
			Model *tmp = new Model(path, m_ModelNumber);
			m_ModelsMapping[path] = tmp;

			m_ModelNumber++;
		}
	}

	void ModelManager::CreateVAO()
	{
		m_ModelsAlreadyLoaded = true;

		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;

		std::map<std::string, Model*>::iterator it = m_ModelsMapping.begin();

		while (it != m_ModelsMapping.end())
		{
			ModelEntry me(it->second->m_ID, it->second->m_TotalNumIndices, vertices.size(), indices.size());
			m_ModelsEntriesMapping[it->first] = me;

			vertices.insert(vertices.end(), it->second->vertices.begin(), it->second->vertices.end());
			indices.insert(indices.end(), it->second->indices.begin(), it->second->indices.end());

			it++;
		}

		VAO = new VertexArray();
		VertexBuffer* vbo = new VertexBuffer(&vertices[0], vertices.size());
		IndexBuffer* ebo = new IndexBuffer(&indices[0], indices.size());

		vbo->SetLayout({
			{ VertexDataType::VEC3F, "position" },
			{ VertexDataType::VEC3F, "normal" },
			{ VertexDataType::VEC2F, "texCoords" },
			{ VertexDataType::VEC4I, "boneIDs" },
			{ VertexDataType::VEC4F, "boneWeights" }
			});

		VAO->AddVertexBuffer(*vbo);
		VAO->AddIndexBuffer(*ebo);

		// load vao and vbo

		
	}

	void ModelManager::Draw()
	{
		glDrawElements(GL_TRIANGLES, VAO->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, 0);
	}

	ModelManager::ModelManager()
	{
	}


	ModelManager::~ModelManager()
	{
	}
}