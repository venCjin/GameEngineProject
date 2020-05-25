#include "pch.h"
#include "BatchRenderer.h"

#include <Gameplay/Components/Transform.h>
#include <Gameplay/Components/Mesh.h>
#include <Renderer/Material.h>
#include <Core/Timer.h>

namespace sixengine {

	BatchRenderer* BatchRenderer::m_BatchRendererInstance = nullptr;

	bool SortModels(RendererCommand* x, RendererCommand* y)
	{
		return x->ModelID < y->ModelID;
	}

	bool SortShaders(RendererCommand* x, RendererCommand* y)
	{
		return x->shader->GetID() < y->shader->GetID();
	}

	BatchRenderer::BatchRenderer(ModelManager* modelManager, TextureArray* textureArray)
		: m_ModelManager(modelManager), m_TextureArray(textureArray), m_IDBO(100 * sizeof(DrawElementsCommand), 6), m_LockManager(true)
	{
	
	}

	BatchRenderer::~BatchRenderer()
	{

	}

	void BatchRenderer::SubmitCommand(GameObject* gameObject, glm::mat4 model)
	{
		RendererCommand* command = new(m_FrameAllocator.Get(m_CommandList.size())) RendererCommand();

		Transform* t = gameObject->GetComponent<Transform>().Get();
		//Transform* t = gameObject->GetComponent<Transform>().Get();

		//command->distance = Distance(&m_PlayerCamera->GetPosition(), &t->getWorldPosition());
		//command->isTranslucent = false;
		command->gameObject = gameObject;

		command->shader = gameObject->GetComponent<Material>()->GetShader();
		if (gameObject->HasComponent<Mesh>())
			command->ModelID = gameObject->GetComponent<Mesh>()->GetModel()->m_ID;

		command->data.textureLayer = gameObject->GetComponent<Material>()->GetTexture();
		command->data.model = model;

		m_CommandList.push_back(command);
	}

	void BatchRenderer::Render()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		std::vector<std::vector<RendererCommand*>> sortedTechniques;
		//Sort Commands by techniques
		sortedTechniques.reserve(m_TechniqueList.size());


		for (int i = 0; i < m_TechniqueList.size(); i++)
		{
			sortedTechniques.push_back(std::vector<RendererCommand*>());
			sortedTechniques[i].reserve(1024);
		}

		unsigned int lastShaderID;
		for (int t = 0; t < sortedTechniques.size(); t++)
		{
			lastShaderID = m_TechniqueList[t]->GetShader()->GetID();

			for (int i = 0; i < m_CommandList.size(); i++)
			{
				if (lastShaderID == m_CommandList[i]->shader->GetID())
				{
					sortedTechniques[t].push_back(m_CommandList[i]);
				}
			}
		}

		// Iterate over techniques

		for (int t = 0; t < sortedTechniques.size(); t++)
		{
			std::vector<RendererCommand*>& commandList = sortedTechniques[t];

			if (commandList.empty()) continue;

			std::vector<glm::mat4> models;
			models.reserve(commandList.size());
			std::vector<glm::vec4> layers;
			layers.reserve(commandList.size());

			models.push_back(m_TechniqueList[t]->GetCamera()->GetViewMatrix());
			models.push_back(m_TechniqueList[t]->GetCamera()->GetProjectionMatrix());
			//Sort Commands by models
			
			std::sort(commandList.begin(), commandList.end(), SortModels);

			bool pass = false;
			unsigned int lastModelID;

			int index = 0;
			for (int i = 0; i < commandList.size(); i++)
			{
				if (commandList[i]->ModelID == -1)
				{
					index++;
					continue;
				}
				else
				{
					lastModelID = commandList[i]->ModelID;
					index = i;
					break;
				}
			}

			if (index < commandList.size())
			{
				unsigned int modelInstanceCounter = 0;
				unsigned int allIntstanceCounter = 0;
				for (int i = index; i < commandList.size(); i++)
				{
					if (commandList[i]->ModelID != lastModelID)
					{
						ModelManager::ModelEntry me = m_ModelManager->GetModelEntry(lastModelID);
						m_RenderCommandList.push_back(
							{ me.NumIndices, modelInstanceCounter, me.BaseIndex, me.BaseVertex, allIntstanceCounter }
						);

						lastModelID = commandList[i]->ModelID;
						allIntstanceCounter += modelInstanceCounter;
						modelInstanceCounter = 0;
						pass = true;
					}

					models.push_back(commandList[i]->data.model);
					layers.push_back(commandList[i]->data.textureLayer);

					modelInstanceCounter++;
				}

				ModelManager::ModelEntry me = m_ModelManager->GetModelEntry(lastModelID);
				m_RenderCommandList.push_back(
					{ me.NumIndices, modelInstanceCounter, me.BaseIndex, me.BaseVertex, allIntstanceCounter }
				);
			}
			

			m_TechniqueList[t]->Render(commandList, models, layers);

			if (!m_RenderCommandList.empty())
			{
				m_LockManager.WaitForLockedRange(m_IDBO.m_Head, m_IDBO.m_Size);

				void* ptr = (unsigned char*)m_IDBO.m_Ptr + m_IDBO.m_Head;
				memcpy(ptr, m_RenderCommandList.data(), m_RenderCommandList.size() * sizeof(m_RenderCommandList[0]));

				m_ModelManager->Bind();
				glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, (void*)m_IDBO.m_Head, m_RenderCommandList.size(), 0);
				glBindVertexArray(0);

				m_LockManager.LockRange(m_IDBO.m_Head, m_IDBO.m_Size);
				m_IDBO.m_Head = (m_IDBO.m_Head + m_IDBO.m_Size) % (m_IDBO.m_Buffering * m_IDBO.m_Size);

				m_RenderCommandList.clear();
			}
		}

		m_CommandList.clear();
	}

	void BatchRenderer::AddTechnique(Technique* technique)
	{
		m_TechniqueList.push_back(technique);
	}

	void BatchRenderer::Configure()
	{
		GLbitfield mapFlags = GL_MAP_WRITE_BIT
			| GL_MAP_PERSISTENT_BIT
			| GL_MAP_COHERENT_BIT;
		GLbitfield createFlags = mapFlags | GL_DYNAMIC_STORAGE_BIT;

		m_ModelManager->Bind();

		glGenBuffers(1, &m_IDBO.m_ID);
		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, m_IDBO.m_ID);
		glBufferStorage(GL_DRAW_INDIRECT_BUFFER, m_IDBO.m_Buffering * m_IDBO.m_Size, 0, createFlags);
		m_IDBO.m_Ptr = glMapBufferRange(GL_DRAW_INDIRECT_BUFFER, 0, m_IDBO.m_Buffering * m_IDBO.m_Size, mapFlags);

		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
		glBindVertexArray(0);

		for (auto tech : m_TechniqueList)
			tech->Start(m_TextureArray);

		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, m_IDBO.m_ID);

		glClearColor(0.3f, 0.3f, 0.3f, 1.0f);

		glEnable(GL_DEPTH_TEST);

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
	}

	void BatchRenderer::Initialize(ModelManager* modelManager, TextureArray* textureArray)
	{
		if (m_BatchRendererInstance == nullptr)
			m_BatchRendererInstance = new BatchRenderer(modelManager, textureArray);
	}

	float BatchRenderer::Distance(glm::vec3* x, glm::vec3* y)
	{
		return (x->x - y->x) * (x->x - y->x) +
			(x->y - y->y) * (x->y - y->y) +
			(x->z - y->z) * (x->z - y->z);
	}
}
