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

	BatchRenderer::BatchRenderer(ModelManager* modelManager, TextureArray* textureArray, Camera* camera)
		: m_ModelManager(modelManager), m_TextureArray(textureArray), m_PlayerCamera(camera)
	{
		glGenBuffers(1, &ssboModels);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboModels);
		glBufferData(GL_SHADER_STORAGE_BUFFER, 10000 * sizeof(glm::mat4), 0, GL_DYNAMIC_COPY);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssboModels);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

		glGenBuffers(1, &ssboLayers);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboLayers);
		glBufferData(GL_SHADER_STORAGE_BUFFER, 10000 * sizeof(glm::vec4), 0, GL_DYNAMIC_COPY);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssboLayers);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

		glGenBuffers(1, &ssboBones);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboBones);
		glBufferData(GL_SHADER_STORAGE_BUFFER, 100 * sizeof(BonesStruct), 0, GL_DYNAMIC_COPY);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, ssboBones);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

		glGenBuffers(1, &idbo);
		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, idbo);
		glBufferData(GL_DRAW_INDIRECT_BUFFER, 100 * sizeof(RendererCommand), 0, GL_DYNAMIC_DRAW);

		for (int i = 0; i < 100; i++)
		{
			m_Transforms.push_back(std::vector<glm::mat4>());
		}
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
		//command->isDebug = false;
		//command->isTranslucent = false;

		command->shader = gameObject->GetComponent<Material>()->GetShader();
		command->model = gameObject->GetComponent<Mesh>()->GetModel();

		command->data.textureLayer = gameObject->GetComponent<Material>()->GetTexture();
		command->data.model = model;
		
		m_CommandList.push_back(command);
	}

	void BatchRenderer::Render()
	{
		m_ModelManager->Bind();

		glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		std::sort(m_CommandList.begin(), m_CommandList.end(), SortShaders);

		std::vector<std::vector<RendererCommand*>> sortedShader;
		sortedShader.reserve(16);
		for (int i = 0; i < 16; i++)
		{
			sortedShader.push_back(std::vector<RendererCommand*>());
			sortedShader[i].reserve(1024);
		}

		unsigned int vectorID = 0;
		unsigned int lastShaderID = m_CommandList[0]->shader->GetID();
		sortedShader[vectorID].push_back(m_CommandList[0]);
		for (int s = 1; s < m_CommandList.size(); s++)
		{
			if (m_CommandList[s]->shader->GetID() != lastShaderID)
			{
				lastShaderID = m_CommandList[s]->shader->GetID();
				vectorID++;
			}

			sortedShader[vectorID].push_back(m_CommandList[s]);
		}

		for (int s = 0; s < sortedShader.size(); s++)
		{
			if (sortedShader[s].size() == 0) continue;
			std::vector<RendererCommand*> &commandList = sortedShader[s];

			std::vector<glm::mat4> models;
			models.reserve(commandList.size());
			std::vector<glm::vec4> layers;
			layers.reserve(commandList.size());

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

			glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboModels);
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, models.size() * sizeof(models[0]), models.data());
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

			glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboLayers);
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, layers.size() * sizeof(layers[0]), layers.data());
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

			if (commandList[0]->shader->IsAnimated())
			{
				float time = Timer::Instance()->ElapsedTime();
				glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboBones);
				unsigned int offsetStep = 100 * sizeof(glm::mat4);
				unsigned int offset = 0;

				for (int i = 0; i < commandList.size(); i++)
				{
					commandList[i]->model->BoneTransform(time + (float)i * 2.0f, m_Transforms[i]);
					glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, m_Transforms[i].size() * sizeof(m_Transforms[i][0]), m_Transforms[i].data());
					offset += offsetStep;
				}
				glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
			}

			glBindBuffer(GL_DRAW_INDIRECT_BUFFER, idbo);
			glBufferSubData(GL_DRAW_INDIRECT_BUFFER, 0, m_RenderCommandList.size() * sizeof(m_RenderCommandList[0]), m_RenderCommandList.data());

			commandList[0]->shader->Bind();
			commandList[0]->shader->SetMat4("projection", m_PlayerCamera->GetProjectionMatrix());
			commandList[0]->shader->SetMat4("view", m_PlayerCamera->GetViewMatrix());

			m_TextureArray->Bind(0);
			commandList[0]->shader->SetInt("textureArray", 0);

			glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, (void*)0, m_RenderCommandList.size(), 0);

			m_RenderCommandList.clear();
		}

		m_CommandList.clear();
	}

	void BatchRenderer::Initialize(ModelManager* modelManager, TextureArray* textureArray, Camera* camera)
	{
		if (m_BatchRendererInstance == nullptr)
			m_BatchRendererInstance = new BatchRenderer(modelManager, textureArray, camera);
	}

	float BatchRenderer::Distance(glm::vec3* x, glm::vec3* y)
	{
		return (x->x - y->x) * (x->x - y->x) +
			(x->y - y->y) * (x->y - y->y) +
			(x->z - y->z) * (x->z - y->z);
	}
}
