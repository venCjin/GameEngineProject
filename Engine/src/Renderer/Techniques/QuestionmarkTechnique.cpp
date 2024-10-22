#include "pch.h"
#include "QuestionmarkTechnique.h"

#include "Renderer/TextureArray.h"
#include "Renderer/BatchRenderer.h"
#include <Gameplay\Components\Questionmark.h>

namespace sixengine {

	QuestionmarkTechnique::QuestionmarkTechnique(Shader* shader)
		: Technique(shader),
		m_Models(1002 * sizeof(glm::mat4), 0),
		m_Layers(1000 * sizeof(glm::vec4) + 1000 * sizeof(float) + 1000 * sizeof(glm::vec4) + 1000 * sizeof(int), 1)
	{


	}

	void QuestionmarkTechnique::Start(TextureArray* textureArray)
	{
		m_Shader->Bind();

		textureArray->Bind(0);
		m_Shader->SetInt("textureArray", 0);
		m_Shader->SetInt("shadowMap1", 3);

		m_Shader->Unbind();
	}

	void QuestionmarkTechnique::StartFrame(std::vector<RendererCommand*>& commandList, std::vector<DrawElementsCommand> drawCommands, std::vector<glm::mat4>& models, std::vector<glm::vec4> layers)
	{
		m_DrawCommands = drawCommands;

		m_Offset = 0;
		m_Size = 0;
		std::vector<float> barFill;
		std::vector<glm::vec4> color;
		std::vector<int> visible;
		std::vector<glm::mat4> models2;
		models2.push_back(models[0]);
		models2.push_back(models[1]);

		std::sort(commandList.begin(), commandList.end(),
			[](const RendererCommand* a, const RendererCommand* b) -> bool
			{
				glm::vec3 posA(a->data.model[3]);
				glm::vec3 posB(b->data.model[3]);

				glm::vec3 camPosition = Camera::ActiveCamera->m_Transform->GetWorldPosition();

				return glm::distance2(camPosition, posA) > glm::distance2(camPosition, posB);
			});

		for (auto command : commandList)
		{
			Questionmark* q = command->gameObject->GetComponent<Questionmark>().Get();
			barFill.push_back(q->fill);
			color.push_back(q->color);
			visible.push_back(q->visable);
			models2.push_back(command->data.model);
		}

		m_Models.Update(models2.data(), models2.size() * sizeof(models2[0]));
		m_Layers.Update(layers.data(), layers.size() * sizeof(layers[0]));
		m_Layers.Update(barFill.data(), barFill.size() * sizeof(barFill[0]), 1000 * sizeof(glm::vec4));
		m_Layers.Update(color.data(), color.size() * sizeof(color[0]), 1000 * sizeof(glm::vec4) + 1000 * sizeof(float));
		m_Layers.Update(visible.data(), visible.size() * sizeof(visible[0]), 1000 * sizeof(glm::vec4) + 1000 * sizeof(float) + 1000 * sizeof(glm::vec4));
	}

	void QuestionmarkTechnique::Render(std::vector<RendererCommand*>& commandList)
	{
		m_Shader->Bind();

		m_Shader->SetVec3("cameraPos", Camera::ActiveCamera->m_Transform->GetWorldPosition());

		m_Shader->SetBool("useParallaxMapping", !Input::IsKeyActive(KeyCode::F3));

		m_Models.Bind();
		m_Layers.Bind();
	}

	void QuestionmarkTechnique::SetLight(Light& light)
	{

	}

	void QuestionmarkTechnique::FinishFrame()
	{
		m_Models.LockAndMovePointer();
		m_Layers.LockAndMovePointer();
	}

}