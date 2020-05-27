#include "pch.h"
#include "BatchRenderer.h"

#include <Gameplay/Components/Transform.h>
#include <Gameplay/Components/Mesh.h>
#include <Renderer/Material.h>
#include <Core/Timer.h>

#include <iomanip>

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
		: m_ModelManager(modelManager), m_TextureArray(textureArray), m_IDBO(100 * sizeof(DrawElementsCommand), 9)
	{
		m_Depth = nullptr;
		m_Skybox = nullptr;
	}

	BatchRenderer::~BatchRenderer()
	{

	}

	void BatchRenderer::NormalizePlane(glm::vec4& plane)
	{
		float t = std::sqrt(
			plane.x * plane.x +
			plane.y * plane.y +
			plane.z * plane.z);

		plane.x /= t;
		plane.y /= t;
		plane.z /= t;
		plane.w /= t;
	}

	void BatchRenderer::CalculateFrustum()
	{
		glm::mat4 clip = Camera::ActiveCamera->GetProjectionMatrix() * Camera::ActiveCamera->GetViewMatrix();
		
		clip = glm::transpose(clip);
		
		// Left plane 
		m_FrustumPlanes[0] = clip[3] + clip[0];
		// Right plane
		m_FrustumPlanes[1] = clip[3] - clip[0];
		// Bottom plane
		m_FrustumPlanes[2] = clip[3] + clip[1];
		// Top plane
		m_FrustumPlanes[3] = clip[3] - clip[1];
		// Near plane
		m_FrustumPlanes[4] = clip[3] + clip[2];
		// Far plane
		m_FrustumPlanes[5] = clip[3] - clip[2];

		for (int i = 0; i < 6; i++)
			NormalizePlane(m_FrustumPlanes[i]);
	}

	bool BatchRenderer::FrustumAABB(glm::vec3 min, glm::vec3 max)
	{
		int out;
		for (int i = 0; i < 6; i++)
		{
			out = 0;
			out += ((glm::dot(m_FrustumPlanes[i], glm::vec4(min.x, min.y, min.z, 1.0f)) < 0.0) ? 1 : 0);
			out += ((glm::dot(m_FrustumPlanes[i], glm::vec4(max.x, min.y, min.z, 1.0f)) < 0.0) ? 1 : 0);
			out += ((glm::dot(m_FrustumPlanes[i], glm::vec4(min.x, max.y, min.z, 1.0f)) < 0.0) ? 1 : 0);
			out += ((glm::dot(m_FrustumPlanes[i], glm::vec4(max.x, max.y, min.z, 1.0f)) < 0.0) ? 1 : 0);
			out += ((glm::dot(m_FrustumPlanes[i], glm::vec4(min.x, min.y, max.z, 1.0f)) < 0.0) ? 1 : 0);
			out += ((glm::dot(m_FrustumPlanes[i], glm::vec4(max.x, min.y, max.z, 1.0f)) < 0.0) ? 1 : 0);
			out += ((glm::dot(m_FrustumPlanes[i], glm::vec4(min.x, max.y, max.z, 1.0f)) < 0.0) ? 1 : 0);
			out += ((glm::dot(m_FrustumPlanes[i], glm::vec4(max.x, max.y, max.z, 1.0f)) < 0.0) ? 1 : 0);
			if (out == 8) return false; // outside
		}

		return true; // inside or intersect
	}

	void BatchRenderer::SubmitCommand(GameObject* gameObject, glm::mat4 model)
	{
		bool render = true;

		ComponentHandle<Mesh> mesh;
		if (gameObject->HasComponent<Mesh>())
		{
			mesh = gameObject->GetComponent<Mesh>();
			glm::mat4 proj = Camera::ActiveCamera->GetProjectionMatrix();
			glm::mat4 view = Camera::ActiveCamera->GetViewMatrix();

			glm::vec3 min = mesh->GetModel()->m_MinAxis;
			glm::vec3 max = mesh->GetModel()->m_MaxAxis;
			min = model * glm::vec4(min, 1.0f);
			max = model * glm::vec4(max, 1.0f);

			render = FrustumAABB(min, max);
		}

		if (render)
		{
			RendererCommand* command = new(m_FrameAllocator.Get(m_CommandList.size())) RendererCommand();

			//command->distance = Distance(&m_PlayerCamera->GetPosition(), &t->getWorldPosition());
			//command->isTranslucent = false;

			command->gameObject = gameObject;

			command->shader = gameObject->GetComponent<Material>()->GetShader();
			
			if (gameObject->HasComponent<Mesh>())
				command->ModelID = mesh->GetModel()->m_ID;

			command->data.textureLayer = gameObject->GetComponent<Material>()->GetTexture();
			command->data.model = model;

			m_CommandList.push_back(command);
		}
	}

	void BatchRenderer::Render()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		LOG_CORE_INFO(m_CommandList.size());

		// Draw skybox
		//***********************************************
		if (m_Skybox)
			RenderSkybox();

		// Draw depth
		//***********************************************
		if (m_Depth)
			RenderDepth(m_CommandList);

		// Draw normal
		//***********************************************
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

			models.push_back(Camera::ActiveCamera->GetViewMatrix());
			models.push_back(Camera::ActiveCamera->GetProjectionMatrix());
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
			m_TechniqueList[t]->SetLight(*m_DirectionalLight);

			if (!m_RenderCommandList.empty())
			{
				m_IDBO.m_LockManager.WaitForLockedRange(m_IDBO.m_Head, m_IDBO.m_Size);

				void* ptr = (unsigned char*)m_IDBO.m_Ptr + m_IDBO.m_Head;
				memcpy(ptr, m_RenderCommandList.data(), m_RenderCommandList.size() * sizeof(m_RenderCommandList[0]));

				m_ModelManager->Bind();
				glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, (void*)m_IDBO.m_Head, m_RenderCommandList.size(), 0);
				glBindVertexArray(0);

				m_IDBO.m_LockManager.LockRange(m_IDBO.m_Head, m_IDBO.m_Size);
				m_IDBO.m_Head = (m_IDBO.m_Head + m_IDBO.m_Size) % (m_IDBO.m_Buffering * m_IDBO.m_Size);

				m_RenderCommandList.clear();
			}
		}
		
		m_CommandList.clear();
	}

	void BatchRenderer::RenderDepth(std::vector<RendererCommand*>& commandList)
	{
		std::vector<RendererCommand*> depth(commandList);

		std::sort(depth.begin(), depth.end(), SortModels);

		std::vector<glm::mat4> models;
		bool pass = false;
		unsigned int lastModelID;

		int index = 0;
		for (int i = 0; i < depth.size(); i++)
		{
			if (depth[i]->ModelID == -1)
			{
				index++;
				continue;
			}
			else
			{
				lastModelID = depth[i]->ModelID;
				index = i;
				break;
			}
		}

		if (index < depth.size())
		{
			unsigned int modelInstanceCounter = 0;
			unsigned int allIntstanceCounter = 0;
			for (int i = index; i < depth.size(); i++)
			{
				if (depth[i]->ModelID != lastModelID)
				{
					ModelManager::ModelEntry me = m_ModelManager->GetModelEntry(lastModelID);
					m_RenderCommandList.push_back(
						{ me.NumIndices, modelInstanceCounter, me.BaseIndex, me.BaseVertex, allIntstanceCounter }
					);

					lastModelID = depth[i]->ModelID;
					allIntstanceCounter += modelInstanceCounter;
					modelInstanceCounter = 0;
					pass = true;
				}

				models.push_back(depth[i]->data.model);

				modelInstanceCounter++;
			}

			ModelManager::ModelEntry me = m_ModelManager->GetModelEntry(lastModelID);
			m_RenderCommandList.push_back(
				{ me.NumIndices, modelInstanceCounter, me.BaseIndex, me.BaseVertex, allIntstanceCounter }
			);
		}


		if (!m_RenderCommandList.empty())
		{
			m_Depth->Render(depth, models, std::vector<glm::vec4>());
			m_Depth->SetLight(*m_DirectionalLight);

			m_IDBO.m_LockManager.WaitForLockedRange(m_IDBO.m_Head, m_IDBO.m_Size);

			void* ptr = (unsigned char*)m_IDBO.m_Ptr + m_IDBO.m_Head;
			memcpy(ptr, m_RenderCommandList.data(), m_RenderCommandList.size() * sizeof(m_RenderCommandList[0]));

			m_DirectionalLight->m_DepthFramebuffer.Bind();

			m_ModelManager->Bind();
			glCullFace(GL_FRONT);
			glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, (void*)m_IDBO.m_Head, m_RenderCommandList.size(), 0);
			glCullFace(GL_BACK);
			glBindVertexArray(0);

			m_DirectionalLight->m_DepthFramebuffer.Unbind();

			m_IDBO.m_LockManager.LockRange(m_IDBO.m_Head, m_IDBO.m_Size);
			m_IDBO.m_Head = (m_IDBO.m_Head + m_IDBO.m_Size) % (m_IDBO.m_Buffering * m_IDBO.m_Size);

			m_RenderCommandList.clear();
		}
	}

	void BatchRenderer::RenderSkybox()
	{
		m_Skybox->Render();
	}

	void BatchRenderer::SetSkybox(SkyboxRender* technique)
	{
		m_Skybox = technique;
	}

	void BatchRenderer::SetDepth(DepthRender* technique)
	{
		m_Depth = technique;
	}

	void BatchRenderer::SetLight(Light* light)
	{
		m_DirectionalLight = light;
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

		glBindVertexArray(0);

		for (auto tech : m_TechniqueList)
			tech->Start(m_TextureArray);

		if (m_Depth)
			m_Depth->Start(m_TextureArray);

		if (m_Skybox)
			m_Skybox->Start(m_TextureArray);

		glClearColor(0.3f, 0.3f, 0.3f, 1.0f);

		glEnable(GL_DEPTH_TEST);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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
