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

		auto transform = GetComponent<Transform>();

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
				unsigned int texture = GetComponent<TestMaterial>()->m_Texture;
				auto mesh = GetComponent<TestMesh>();
				//Renderer::Render(mesh->VAO, shader);
				Renderer::Render(mesh->VAO, shader, texture);
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
/*=======
		auto mesh = GetComponent<TestMesh>();
		auto shader = GetComponent<TestMaterial>()->GetShader();
		unsigned int texture = GetComponent<TestMaterial>()->m_Texture;
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
		//shader->SetInt("texture1", texture);
		Renderer::Render(mesh->VAO, shader, texture);
>>>>>>> 3f1ed511ba35e7f1532f13a7a6d29d3e9c7b3893*/

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