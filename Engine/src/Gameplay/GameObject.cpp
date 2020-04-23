#pragma once

#include <pch.h>
#include "GameObject.h"
#include "Gameplay/GameObject.h"
#include "Gameplay/Components/Transform.h"
#include "Gameplay/Components/TestMaterial.h"
#include "Gameplay/Components/TestMesh.h"
#include "Renderer/Model.h"
#include "Renderer/Renderer.h"
#include "Core/Timer.h"

namespace sixengine {

	void GameObject::Render(glm::mat4 projection, glm::mat4 view)
	{
		Render(projection, view, Transform(this), false);
	}

	void GameObject::Render(glm::mat4 projection, glm::mat4 view, Transform parentWorld, bool dirty)
	{
		// TODO: Render Queue
		//Renderer::AddToRenderQueue(this);
		//parentWorld;
		auto transform = GetComponent<Transform>();
		//transform;
		if (HasComponent<TestMaterial>())
		{
			auto shader = GetComponent<TestMaterial>()->GetShader();

			shader->Bind();
			shader->SetMat4("projection", projection);
			shader->SetMat4("view", view);

			dirty |= m_Dirty;
			if (dirty)
			{
				shader->SetMat4("model", transform->Combine(parentWorld));
				m_Dirty = false;
			}
			else
			{
				shader->SetMat4("model", transform->Combine());
			}
		
			if (HasComponent<TestMesh>())
			{
				auto mesh = GetComponent<TestMesh>();
				Renderer::Render(mesh->VAO, shader);
			}
			else if (HasComponent<Model>())
			{
				auto model = GetComponent<Model>();
				auto& transforms = model->GetCurrentTransforms();
				float time = Timer::Instance()->ElapsedTime();
				model->BoneTransform(time, transforms);
				for (int i = 0; i < transforms.size(); i++)
				{
					shader->SetMat4("gBones[" + std::to_string(i) + "]", transforms[i]);
				}

				model->Render(shader);
			}
		}

		for (auto child : m_Childeren)
		{
			child->Render(projection, view, *transform, dirty);
		}
	}

	void GameObject::SetDirty(bool dirty)
	{
		m_Dirty = dirty;
	}

	void GameObject::AddChild(GameObject* child)
	{
		m_Childeren.push_back(child);
	}

	std::vector<GameObject*> GameObject::GetChildren()
	{
		return m_Childeren;
	}

}