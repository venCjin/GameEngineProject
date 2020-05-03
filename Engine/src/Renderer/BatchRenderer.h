#pragma once

#include <Gameplay/GameObject.h>
#include <Renderer/TextureArray.h>
#include <Core/Camera.h>

namespace sixengine {

	struct RendererCommand
	{
		float distance;
		unsigned char materialID;
		unsigned char meshID;
		bool isTranslucent;
		bool isDebug;

		GameObject* object;
	};

	struct DrawElementsCommand
	{
		unsigned int vertexCount;
		unsigned int instanceCount;
		unsigned int firstIndex;
		unsigned int baseVertex;
		unsigned int baseInstance;
	};

	class BatchRenderer
	{
	private:
		std::vector<RendererCommand*> m_CommandList;
		std::vector<DrawElementsCommand*> m_RenderCommandList;

		TextureArray* m_TextureArray;
		Camera* m_PlayerCamera;

	public:
		BatchRenderer(TextureArray* textureArray, Camera* camera);
		~BatchRenderer();

		void SubmitCommand(GameObject* gameObject); //how to add gizmo?
		void Render();

	private:
		int Compare(const void* x, const void* y);
		float Distance(glm::vec3* x, glm::vec3* y);
	};

}
