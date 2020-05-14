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
		return x->model->m_ID < y->model->m_ID;
	}

	bool SortShaders(RendererCommand* x, RendererCommand* y)
	{
		return x->shader->GetID() < y->shader->GetID();
	}

	BatchRenderer::BatchRenderer(ModelManager* modelManager, TextureArray* textureArray)
		: m_ModelManager(modelManager), m_TextureArray(textureArray)
	{
		glGenBuffers(1, &m_IDBO);
		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, m_IDBO);
		glBufferData(GL_DRAW_INDIRECT_BUFFER, 100 * sizeof(RendererCommand), 0, GL_DYNAMIC_DRAW);

		glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
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

		command->model = gameObject->GetComponent<Mesh>()->GetModel();
		command->data.model = model;

		command->shader = gameObject->GetComponent<Material>()->GetShader();
		command->data.textureLayer = gameObject->GetComponent<Material>()->GetTexture();

		m_CommandList.push_back(command);
	}

	void BatchRenderer::Render()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

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

			//Sort Commands by models
			std::vector<glm::mat4> models;
			models.reserve(commandList.size());
			std::vector<glm::vec4> layers;
			layers.reserve(commandList.size());

			models.push_back(m_TechniqueList[t]->GetCamera()->GetViewMatrix());
			models.push_back(m_TechniqueList[t]->GetCamera()->GetProjectionMatrix());

			std::sort(commandList.begin(), commandList.end(), SortModels);

			bool pass = false;
			unsigned int lastModelID = commandList[0]->model->m_ID;
			unsigned int modelInstanceCounter = 0;
			unsigned int allIntstanceCounter = 0;
			for (int i = 0; i < commandList.size(); i++)
			{
				if (commandList[i]->model->m_ID != lastModelID)
				{
					ModelManager::ModelEntry me = m_ModelManager->GetModelEntry(lastModelID);
					m_RenderCommandList.push_back(
						{ me.NumIndices, modelInstanceCounter, me.BaseIndex, me.BaseVertex, allIntstanceCounter }
					);

					lastModelID = commandList[i]->model->m_ID;
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

			m_TechniqueList[t]->Render(commandList, models, layers);

			glBufferSubData(GL_DRAW_INDIRECT_BUFFER, 0, m_RenderCommandList.size() * sizeof(m_RenderCommandList[0]), m_RenderCommandList.data());
			
			m_ModelManager->Bind();

			glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, (void*)0, m_RenderCommandList.size(), 0);

			m_RenderCommandList.clear();
		}

		m_CommandList.clear();
	}

	void BatchRenderer::AddTechnique(Technique* technique)
	{
		m_TechniqueList.push_back(technique);
	}

	void BatchRenderer::Configure()
	{
		m_ModelManager->Bind();
		for (auto tech : m_TechniqueList)
			tech->Start(m_TextureArray);
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
