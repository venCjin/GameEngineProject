#include "pch.h"
#include "DepthRender.h"

namespace sixengine {



	DepthRender::DepthRender(Shader* shader)
		: Technique(shader), m_Models(2000 * sizeof(glm::mat4), 6)
	{
	}

	void DepthRender::Start(TextureArray* textureArray)
	{
		/*lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
		lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
		lightSpaceMatrix = lightProjection * lightView;*/
	}

	void DepthRender::Render(std::vector<RendererCommand*>& commandList, std::vector<glm::mat4>& models, std::vector<glm::vec4> layers)
	{
		m_Shader->Bind();

		m_Models.Update(models.data(), models.size() * sizeof(models[0]));

		glm::mat4 lightProjection = glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f, 1.0f, 50.0f);
		glm::mat4 lightView = glm::lookAt(glm::vec3(-2.0f, 24.0f, -1.0f),
										  glm::vec3( 0.0f, 0.0f,  0.0f),
										  glm::vec3( 0.0f, 1.0f,  0.0f));

		glm::mat4 lightSpaceMatrix = lightProjection * lightView;

		m_Shader->SetMat4("lightSpaceMatrix", lightSpaceMatrix);
	}

}