#pragma once

#include "Gameplay/GameObject.h"
#include "Core/CameraSystem/Camera.h"
#include "Core/ShaderManager.h"
#include "Renderer/BatchRenderer.h"
#include "Renderer/ModelManager.h"
#include "Renderer/MaterialManager.h"
#include "Renderer/TextureArray.h"
#include "Renderer/Techniques/Technique.h"

namespace sixengine {

	class Scene
	{
	private:
		GameObject *m_UIRoot, *m_SceneRoot;
		ModelManager* m_ModelManager;
		ShaderManager* m_ShaderManager;
		MaterialManager * m_MaterialManager;
		TextureArray* m_TextureArray;
		BatchRenderer* m_BatchRenderer;
	public:
		Scene(unsigned int width, unsigned int height);
		~Scene();
		bool LoadScene(const char* filePath);
		bool SaveScene(const char* filePath);
		void Render(bool first = false);
		void DrawGizmos();
	private:
		GameObject* ReadGameObject(std::fstream& file, EntityManager& en);
	};

}
