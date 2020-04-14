#pragma once

#include <pch.h>
#include "GameObject.h"
#include "Gameplay/GameObject.h"
#include "Gameplay/Components/Transform.h"
#include "Gameplay/Components/TestMaterial.h"
#include "Gameplay/Components/TestMesh.h"
#include "Renderer/Renderer.h"

namespace sixengine {

	void GameObject::Render(glm::mat4 projection, glm::mat4 view)
	{
		Render(projection, view, Transform(this), false);
	}

	void GameObject::Render(glm::mat4 projection, glm::mat4 view, Transform parentWorld, bool dirty)
	{
		auto& t = GetComponent<Transform>();

		// TODO: Render Queue
		//Renderer::AddToRenderQueue(this);

		auto transform = GetComponent<Transform>();
		auto mesh = GetComponent<TestMesh>();
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

		Renderer::Render(mesh->VAO, shader);

		for (auto child : m_Childeren)
		{
			child->Render(projection, view, *t, dirty);
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