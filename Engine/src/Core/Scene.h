#pragma once

#include "Gameplay/GameObject.h"
#include "Core/CameraSystem/Camera.h"
#include "Core/ShaderManager.h"
#include "Renderer/BatchRenderer.h"
#include "Renderer/ModelManager.h"
#include "Renderer/MaterialManager.h"
#include "Renderer/TextureArray.h"
#include "Renderer/Techniques/Technique.h"

#define SCENE_ENDS_IN_GAME_CPP 1

namespace sixengine {

	class Scene
	{
	private:
	public:
		GameObject *m_UIRoot, *m_SceneRoot;
		ModelManager* m_ModelManager;
		ShaderManager* m_ShaderManager;
		MaterialManager* m_MaterialManager;
		TextureArray* m_TextureArray;
		BatchRenderer* m_BatchRenderer;
	
		Scene(unsigned int width, unsigned int height);
		~Scene();
		bool LoadScene(const char* filePath);
		bool SaveScene(const char* filePath);
		void Render(bool first = false);
		void DrawGizmos();
		template <typename Component>
		GameObject* GetFirstGameObjectWithComponent();
		template <typename Component>
		std::vector<GameObject*> GetGameObjectsWithComponent();
	private:
		GameObject* ReadGameObject(std::fstream& file, EntityManager& en);
		GameObject* ReadEnemy(std::fstream& file, EntityManager& en);
	};

	template<typename Component>
	GameObject* Scene::GetFirstGameObjectWithComponent()
	{
		GameObject* result;
		m_SceneRoot->GetFirstChildWithComponent<Component>(result);
		return result;
	}

	template <typename Component>
	std::vector<GameObject*> Scene::GetGameObjectsWithComponent()
	{
		std::vector<GameObject*> result;
		m_SceneRoot->GetChildrenWithComponent<Component>(result);
		return result;
	}

}
