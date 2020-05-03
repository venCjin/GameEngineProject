#include "pch.h"
#include "BatchRenderer.h"

#include <Gameplay/Components/Transform.h>

namespace sixengine {

	BatchRenderer::BatchRenderer(TextureArray* textureArray, Camera* camera)
		: m_TextureArray(textureArray), m_PlayerCamera(camera)
	{
		  
	}

	BatchRenderer::~BatchRenderer()
	{

	}

	void BatchRenderer::SubmitCommand(GameObject* gameObject)
	{
		RendererCommand* command= new RendererCommand();

		Transform* t = gameObject->GetComponent<Transform>().Get();
		//Transform* t = gameObject->GetComponent<Transform>().Get();

		command->distance = Distance(&m_PlayerCamera->GetPosition(), &t->getWorldPosition());
		command->isDebug = false;
		command->isTranslucent = false;
		//command->materialID = ;
		//command->meshID = ;

		m_CommandList.push_back(command);
	}

	void BatchRenderer::Render()
	{

	}

	float BatchRenderer::Distance(glm::vec3* x, glm::vec3* y)
	{
		return (x->x - y->x) * (x->x - y->x) +
			(x->y - y->y) * (x->y - y->y) +
			(x->z - y->z) * (x->z - y->z);
	}

	int BatchRenderer::Compare(const void* x, const void* y)
	{
		const float* xf = (float*) x;
		const float* yf = (float*) y;

		if (*xf > *yf)
			return 1;
		else if (*xf < *yf)
			return -1;

		return 0;
	}

}
