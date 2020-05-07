#include "pch.h"
#include "ModelManager.h"

namespace sixengine {

	ModelManager::ModelManager()
	{

	}

	ModelManager::~ModelManager()
	{

	}

	void ModelManager::AddModel(std::string path)
	{
		std::size_t slash = path.find_last_of("/\\");
		std::size_t dot = path.find_last_of(".");
		std::string modelName = path.substr(slash + 1, dot - slash - 1);

		if (!m_ModelsAlreadyLoaded && m_ModelsMapping.find(modelName) == m_ModelsMapping.end())
		{
			Model *tmp = new Model(path, m_ModelNumber);
			m_ModelsMapping[modelName] = tmp;

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
			ModelEntry me(it->second->m_TotalNumIndices, vertices.size(), indices.size());
			m_ModelsEntriesMapping[it->second->m_ID] = me;

			vertices.insert(vertices.end(), it->second->vertices.begin(), it->second->vertices.end());
			indices.insert(indices.end(), it->second->indices.begin(), it->second->indices.end());

			it++;
		}

		ModelEntry tmp = m_ModelsEntriesMapping[2];

		m_VAO = new VertexArray();
		VertexBuffer* vbo = new VertexBuffer(&vertices[0], vertices.size());
		IndexBuffer* ebo = new IndexBuffer(&indices[0], indices.size());

		vbo->SetLayout({
			{ VertexDataType::VEC3F, "position" },
			{ VertexDataType::VEC3F, "normal" },
			{ VertexDataType::VEC2F, "texCoords" },
			{ VertexDataType::VEC4I, "boneIDs" },
			{ VertexDataType::VEC4F, "boneWeights" }
		});

		m_VAO->AddVertexBuffer(*vbo);
		m_VAO->AddIndexBuffer(*ebo);
	}

	void ModelManager::Bind()
	{
		m_VAO->Bind();
	}
}