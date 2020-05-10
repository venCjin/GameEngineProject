#pragma once

#include "Model.h" 
#include "Buffers.h"
#include "VertexArray.h"

#include <map>
#include <vector>

namespace sixengine {

	class ModelManager
	{
	public:
		struct ModelEntry
		{
			ModelEntry()
			{
				BaseVertex = 0;
				BaseIndex = 0;
				NumIndices = 0;
			}

			ModelEntry(unsigned int numIndices, unsigned int baseVertex, unsigned int baseIndex)
			{
				BaseVertex = baseVertex;
				BaseIndex = baseIndex;
				NumIndices = numIndices;
			}

			unsigned int BaseVertex;
			unsigned int BaseIndex;
			unsigned int NumIndices;
		};

	public:
		Model* AddModel(std::string path);
		void CreateVAO();	// loads all models in m_ModelPaths and then creates one VAO

		void Bind();

		inline Model* GetModel(std::string modelName) { return m_ModelsMapping[modelName]; }
		inline ModelEntry GetModelEntry(unsigned int modelID) { return m_ModelsEntriesMapping[modelID]; }
		inline unsigned int GetNumberOfModels() { return m_ModelNumber - 1; }

		ModelManager();
		~ModelManager();

	private:
		VertexArray* m_VAO;

		std::map<std::string, Model*> m_ModelsMapping;
		std::map<unsigned int, ModelEntry> m_ModelsEntriesMapping;

		bool m_ModelsAlreadyLoaded = false;
		unsigned int m_ModelNumber = 0;
	};
}