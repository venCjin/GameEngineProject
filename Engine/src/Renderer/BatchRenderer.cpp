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
		: m_ModelManager(modelManager),
		m_TextureArray(textureArray),
		m_IDBO(1000 * sizeof(DrawElementsCommand)),
		m_Default(Application::Get().GetWindow().GetWidth(), Application::Get().GetWindow().GetHeight(),
			GL_TEXTURE_2D, GL_LINEAR, GL_RGBA16F, GL_RGBA, false, GL_COLOR_ATTACHMENT0, true),
		m_PostProcess(Application::Get().GetWindow().GetWidth(), Application::Get().GetWindow().GetHeight(),
			GL_TEXTURE_2D, GL_LINEAR, GL_RGBA16F, GL_RGBA, false, GL_COLOR_ATTACHMENT0, false)
	{
		m_Offset = 0;
		m_DepthStatic = nullptr;
		m_DepthAnimated = nullptr;
		m_Skybox = nullptr; 
		m_ParticleRender = nullptr;
		m_Water = nullptr;
		m_BlurShader = nullptr;
		
		unsigned int VBO;
		float vertices[] = {
			// pos        // tex
			-1.0f, -1.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 1.0f, 1.0f,
			-1.0f,  1.0f, 0.0f, 1.0f,

			-1.0f, -1.0f, 0.0f, 0.0f,
			 1.0f, -1.0f, 1.0f, 0.0f,
			 1.0f,  1.0f, 1.0f, 1.0f
		};

		glGenVertexArrays(1, &this->m_QuadVAO);
		glBindVertexArray(this->m_QuadVAO);

		glGenBuffers(1, &VBO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);


		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
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

		if (gameObject->HasComponent<ParticleEmitter>())
			m_ParticleList.push_back(gameObject);

		if (!gameObject->HasComponent<Mesh>() && !gameObject->HasComponent<Material>())
		{
			render = false;
		}

		ComponentHandle<Mesh> mesh;
		if (gameObject->HasComponent<Mesh>() && gameObject->GetComponent<Mesh>()->GetModel())
		{
			mesh = gameObject->GetComponent<Mesh>();

			glm::vec3 min = mesh->GetModel()->m_MinAxis;
			glm::vec3 max = mesh->GetModel()->m_MaxAxis;
			glm::mat4 tmpModel = glm::scale(model, glm::vec3(2.5f, 2.5f, 2.5f));
			min = tmpModel * glm::vec4(min, 1.0f);
			max = tmpModel * glm::vec4(max, 1.0f);

			render = FrustumAABB(min, max);
		}
		else if (gameObject->HasComponent<Text>())
		{
			render = true;
		}

		if (render)
		{
			RendererCommand* command = new(m_FrameAllocator.Get(m_CommandList.size())) RendererCommand();

			//command->distance = Distance(&m_PlayerCamera->GetPosition(), &t->getWorldPosition());
			//command->isTranslucent = false;

			command->gameObject = gameObject;

			command->shader = gameObject->GetComponent<Material>()->GetShader();
			command->data.textureLayer = gameObject->GetComponent<Material>()->GetTexture();

			if (gameObject->HasComponent<Mesh>())
				command->ModelID = mesh->GetModel()->m_ID;

			command->data.model = model;

			m_CommandList.push_back(command);
		}
	}

	void BatchRenderer::Render()
	{
		//LOG_CORE_INFO(m_CommandList.size());

		m_IDBO.m_LockManager.WaitForLockedRange(m_IDBO.m_Head, m_IDBO.m_Size);

		// Prepare data
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

			if (commandList.empty()) { m_TechniqueList[t]->SetVisible(false); continue; }
			else { m_TechniqueList[t]->SetVisible(true); }

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

			m_TechniqueList[t]->StartFrame(commandList, m_RenderCommandList, models, layers);
			m_TechniqueList[t]->SetLight(*m_DirectionalLight);

			m_RenderCommandList.clear();
		}

		// Light shadow box update
		//***********************************************
		m_DirectionalLight->Update();

		// Draw depth
		//***********************************************

		m_DirectionalLight->m_DepthFramebuffer.Bind();

		if (m_DepthStatic)
			RenderDepth(m_DepthStatic, m_TechniqueList[0]);

		if (m_DepthAnimated)
			RenderDepth(m_DepthAnimated, m_TechniqueList[1]);

		m_DirectionalLight->m_DepthFramebuffer.Unbind();

		// Draw skybox
		//***********************************************
		if (m_Blur)
			m_Default.BindTarget();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (m_Skybox)
			RenderSkybox();

		// Draw water
		//***********************************************
		if (m_Water && m_Water->IsVisible())
			RenderWater(m_TechniqueList[0], m_TechniqueList[1]);

		if (m_Blur)
			m_Default.BindTarget();

		// Draw normal
		//***********************************************
		for (int i = 0; i < sortedTechniques.size() - 1; i++)
		{
			m_TechniqueList[i]->Render(sortedTechniques[i]);

			if (!m_TechniqueList[i]->m_DrawCommands.empty())
			{
				void* ptr = (unsigned char*)m_IDBO.m_Ptr + m_IDBO.m_Head + m_Offset;
				memcpy(ptr, m_TechniqueList[i]->m_DrawCommands.data(), m_TechniqueList[i]->m_DrawCommands.size() * sizeof(DrawElementsCommand));

				m_ModelManager->Bind();
				glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, (void*)(m_IDBO.m_Head + m_Offset), m_TechniqueList[i]->m_DrawCommands.size(), 0);
				glBindVertexArray(0);

				m_Offset += m_TechniqueList[i]->m_DrawCommands.size() * sizeof(DrawElementsCommand);
				m_TechniqueList[i]->m_DrawCommands.clear();
			}
		}

		if (m_Blur)
			ApplyBlur();

		{
			int i = m_TechniqueList.size() - 1;
			m_TechniqueList[i]->Render(sortedTechniques[i]);

			if (!m_TechniqueList[i]->m_DrawCommands.empty())
			{
				void* ptr = (unsigned char*)m_IDBO.m_Ptr + m_IDBO.m_Head + m_Offset;
				memcpy(ptr, m_TechniqueList[i]->m_DrawCommands.data(), m_TechniqueList[i]->m_DrawCommands.size() * sizeof(DrawElementsCommand));

				m_ModelManager->Bind();
				glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, (void*)(m_IDBO.m_Head + m_Offset), m_TechniqueList[i]->m_DrawCommands.size(), 0);
				glBindVertexArray(0);

				m_Offset += m_TechniqueList[i]->m_DrawCommands.size() * sizeof(DrawElementsCommand);
				m_TechniqueList[i]->m_DrawCommands.clear();
			}
		}

		if (m_ParticleRender)
			m_ParticleRender->Render(m_ParticleList);

		for (auto t : m_TechniqueList)
			t->FinishFrame();

		m_IDBO.m_LockManager.LockRange(m_IDBO.m_Head, m_IDBO.m_Size);
		m_IDBO.m_Head = (m_IDBO.m_Head + m_IDBO.m_Size) % (m_IDBO.m_Buffering * m_IDBO.m_Size);

		m_CommandList.clear();
		m_Offset = 0;

		m_ParticleList.clear();
	}

	void BatchRenderer::RenderDepth(Technique* depth, Technique* technique)
	{
		technique->Render(m_CommandList);

		if (!technique->m_DrawCommands.empty())
		{
			depth->Render(m_CommandList);
			depth->SetLight(*m_DirectionalLight);

			void* ptr = (unsigned char*)m_IDBO.m_Ptr + m_IDBO.m_Head + m_Offset;
			memcpy(ptr, technique->m_DrawCommands.data(), technique->m_DrawCommands.size() * sizeof(DrawElementsCommand));

			m_ModelManager->Bind();
			glCullFace(GL_FRONT);
			glPolygonMode(GL_BACK, GL_FILL);

			glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, (void*)(m_IDBO.m_Head + m_Offset), technique->m_DrawCommands.size(), 0);

			glCullFace(GL_BACK);
			glBindVertexArray(0);

			m_Offset += technique->m_DrawCommands.size() * sizeof(DrawElementsCommand);

		}
	}


	void BatchRenderer::ApplyBlur()
	{
		RenderTarget::BindDefaultTarget();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		m_BlurShader->Bind();
		m_BlurShader->SetFloat("time", Timer::Instance()->ElapsedTime());

		glActiveTexture(GL_TEXTURE2);
		m_Default.BindTexture();

		RenderQuad();

		m_BlurShader->Unbind();
	}

	void BatchRenderer::RenderSkybox()
	{
		m_Skybox->Render();
	}

	void BatchRenderer::SetParticle(ParticleRender* technique)
	{
		m_ParticleRender = technique;
	}

	void BatchRenderer::RenderWater(Technique* technique1, Technique* technique2)
	{
		glm::vec4 clipUp(0.0f, 1.0f, 0.0f, -m_Water->GetGameObject().GetComponent<Transform>()->GetWorldPosition().y + 0.1f);
		glm::vec4 clipDown(0.0f, -1.0f, 0.0f, m_Water->GetGameObject().GetComponent<Transform>()->GetWorldPosition().y + 0.1f);
		Camera temp = m_Water->GetReflectCamera();
		temp.m_Transform->SetWorld(Camera::ActiveCamera->m_Transform->GetWorldCopy());
		temp.m_Transform->SetWorldRotation(Camera::ActiveCamera->m_Transform->GetWorldRotation());
		temp.m_Transform->SetLocalScale(Camera::ActiveCamera->m_Transform->GetLocalScale());
		glEnable(GL_CLIP_DISTANCE0);

		// frame buffer 1 - reflect
		float distance = 2 * (temp.m_Transform->GetWorldPosition().y - m_Water->GetGameObject().GetComponent<Transform>()->GetWorldPosition().y);
		temp.m_Transform->Translate(0.0f, -distance, 0.0f);

		glm::vec3 ori = temp.m_Transform->GetWorldOrientation();
		if( abs(glm::dot(glm::vec3(0.0f, 0.0f, -1.0f), ori)) < 10.0f )
			temp.m_Transform->SetLocalOrientation(ori.x, -ori.y, ori.z);
		else
			temp.m_Transform->SetLocalOrientation(-ori.x, ori.y, -ori.z);

		m_Water->GetFrameBuffers().BindReflectionFramebuffer();
		glClear(GL_COLOR_BUFFER_BIT);
		glClear(GL_DEPTH_BUFFER_BIT);
		technique1->Render(m_CommandList);
		technique1->GetShader()->SetVec4("clipPlane", clipUp);
		technique1->GetShader()->SetFloat("isWater", 1.0f);
		technique1->GetShader()->SetMat4("waterView", temp.GetViewMatrix());
		if (!technique1->m_DrawCommands.empty())
		{
			void* ptr = (unsigned char*)m_IDBO.m_Ptr + m_IDBO.m_Head + m_Offset;
			memcpy(ptr, technique1->m_DrawCommands.data(), technique1->m_DrawCommands.size() * sizeof(DrawElementsCommand));

			m_ModelManager->Bind();

			glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, (void*)(m_IDBO.m_Head + m_Offset), technique1->m_DrawCommands.size(), 0);

			glBindVertexArray(0);

			m_Offset += technique1->m_DrawCommands.size() * sizeof(DrawElementsCommand);
		}

		technique2->Render(m_CommandList);
		technique2->GetShader()->SetVec4("clipPlane", clipUp);
		technique2->GetShader()->SetFloat("isWater", 1.0f);
		technique2->GetShader()->SetMat4("waterView", temp.GetViewMatrix());
		if (!technique2->m_DrawCommands.empty())
		{
			void* ptr = (unsigned char*)m_IDBO.m_Ptr + m_IDBO.m_Head + m_Offset;
			memcpy(ptr, technique2->m_DrawCommands.data(), technique2->m_DrawCommands.size() * sizeof(DrawElementsCommand));

			m_ModelManager->Bind();

			glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, (void*)(m_IDBO.m_Head + m_Offset), technique2->m_DrawCommands.size(), 0);

			glBindVertexArray(0);

			m_Offset += technique2->m_DrawCommands.size() * sizeof(DrawElementsCommand);
		}		


		// frame buffer 2 - refract
		m_Water->GetFrameBuffers().BindRefractionFramebuffer();
		glClear(GL_COLOR_BUFFER_BIT);
		glClear(GL_DEPTH_BUFFER_BIT);
		technique1->Render(m_CommandList);
		technique1->GetShader()->SetVec4("clipPlane", clipDown);
		technique1->GetShader()->SetFloat("isWater", 1.0f);
		technique1->GetShader()->SetMat4("waterView", Camera::ActiveCamera->GetViewMatrix());
		if (!technique1->m_DrawCommands.empty())
		{
			void* ptr = (unsigned char*)m_IDBO.m_Ptr + m_IDBO.m_Head + m_Offset;
			memcpy(ptr, technique1->m_DrawCommands.data(), technique1->m_DrawCommands.size() * sizeof(DrawElementsCommand));

			m_ModelManager->Bind();

			glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, (void*)(m_IDBO.m_Head + m_Offset), technique1->m_DrawCommands.size(), 0);

			glBindVertexArray(0);

			m_Offset += technique1->m_DrawCommands.size() * sizeof(DrawElementsCommand);
		}

		technique2->Render(m_CommandList);
		technique2->GetShader()->SetVec4("clipPlane", clipDown);
		technique2->GetShader()->SetFloat("isWater", 1.0f);
		technique2->GetShader()->SetMat4("waterView", Camera::ActiveCamera->GetViewMatrix());
		if (!technique2->m_DrawCommands.empty())
		{
			void* ptr = (unsigned char*)m_IDBO.m_Ptr + m_IDBO.m_Head + m_Offset;
			memcpy(ptr, technique2->m_DrawCommands.data(), technique2->m_DrawCommands.size() * sizeof(DrawElementsCommand));

			m_ModelManager->Bind();

			glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, (void*)(m_IDBO.m_Head + m_Offset), technique2->m_DrawCommands.size(), 0);

			glBindVertexArray(0);

			m_Offset += technique2->m_DrawCommands.size() * sizeof(DrawElementsCommand);
		}

		technique1->GetShader()->Bind();
		technique1->GetShader()->SetFloat("isWater", 0.0f);
		
		technique2->GetShader()->Bind();
		technique2->GetShader()->SetFloat("isWater", 0.0f);
		glDisable(GL_CLIP_DISTANCE0);
		m_Water->GetFrameBuffers().Unbind();
	}

	void BatchRenderer::RenderQuad()
	{
		glBindVertexArray(m_QuadVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	void BatchRenderer::SetBlurShader(Shader* blur)
	{
		m_BlurShader = blur;
	}

	void BatchRenderer::SetBlur(bool blur)
	{
		m_Blur = blur;
	}

	void BatchRenderer::SetSkybox(SkyboxRender* technique)
	{
		m_Skybox = technique;
	}

	void BatchRenderer::SetStaticDepth(DepthRender* technique)
	{
		m_DepthStatic = technique;
	}

	void BatchRenderer::SetAnimatedDepth(DepthRender* technique)
	{
		m_DepthAnimated = technique;
	}

	void BatchRenderer::SetWater(Water* technique)
	{
		m_Water = technique;
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

		if (m_DepthStatic)
			m_DepthStatic->Start(m_TextureArray);

		if (m_DepthAnimated)
			m_DepthAnimated->Start(m_TextureArray);

		if (m_Skybox)
			m_Skybox->Start(m_TextureArray);
	
		if (m_ParticleRender)
			m_ParticleRender->Start(m_TextureArray);

		glClearColor(0.3f, 0.3f, 0.3f, 1.0f);

		glEnable(GL_DEPTH_TEST);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		if (m_BlurShader)
		{
			m_BlurShader->Bind();
			m_BlurShader->SetInt("image", 2);

			float offset = 1.0f / 300.0f;
			float offsets[9][2] = {
				{ -offset,  offset  },  // top-left
				{  0.0f,    offset  },  // top-center
				{  offset,  offset  },  // top-right
				{ -offset,  0.0f    },  // center-left
				{  0.0f,    0.0f    },  // center-center
				{  offset,  0.0f    },  // center - right
				{ -offset, -offset  },  // bottom-left
				{  0.0f,   -offset  },  // bottom-center
				{  offset, -offset  }   // bottom-right    
			};
			glUniform2fv(glGetUniformLocation(m_BlurShader->GetID(), "offsets"), 9, (float*)offsets);

			float blur_kernel[9] = {
				1.0f / 16.0f, 2.0f / 16.0f, 1.0f / 16.0f,
				2.0f / 16.0f, 4.0f / 16.0f, 2.0f / 16.0f,
				1.0f / 16.0f, 2.0f / 16.0f, 1.0f / 16.0f
			};
			glUniform1fv(glGetUniformLocation(m_BlurShader->GetID(), "blur_kernel"), 9, blur_kernel);

			m_BlurShader->Unbind();
		}
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
