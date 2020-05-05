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
		void AddModel(std::string path);
		void CreateVAO();	// loads all models in m_ModelPaths and then creates one VAO
		void Draw();

		ModelManager();
		~ModelManager();
	private:

		struct ModelEntry
		{
			ModelEntry() 
			{
				ModelIndex = 0;
				BaseVertex = 0;
				BaseIndex = 0;
				NumIndices = 0;
			}
			
			ModelEntry(unsigned int modelIndex, unsigned int numIndices, unsigned int baseVertex, unsigned int baseIndex)
			{
				ModelIndex = modelIndex;
				BaseVertex = baseVertex;
				BaseIndex = baseIndex;
				NumIndices = numIndices;
			}

			unsigned int ModelIndex;
			unsigned int BaseVertex;
			unsigned int BaseIndex;
			unsigned int NumIndices;
		};

		VertexArray* VAO;

		//std::vector<Model*> m_Models;
		std::map<std::string, Model*> m_ModelsMapping;	// change it to map so we do not add same model twice?
		std::map<std::string, ModelEntry> m_ModelsEntriesMapping;
		bool m_ModelsAlreadyLoaded = false;
		unsigned int m_ModelNumber = 0;

	};
}