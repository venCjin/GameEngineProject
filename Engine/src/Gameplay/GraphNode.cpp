#pragma once

#include "pch.h"
#include "GraphNode.h"
#include <vector>
#include "Gameplay\GameObject.h"
#include "Gameplay\Components\Transform.h"
#include "Gameplay\Components\TestMaterial.h"
#include "Gameplay\Components\TestMesh.h"
#include "Renderer\Renderer.h"


namespace sixengine {

	GraphNode::GraphNode(GameObject* go) : m_Dirty(true), m_GO(go)
	{}

	void GraphNode::Render(glm::mat4 projection, glm::mat4 view)
	{
		Render(projection, view, Transform(this), false);
	}

	void GraphNode::Render(glm::mat4 projection, glm::mat4 view, Transform parentWorld, bool dirty)
	{
		auto& t = m_GO->GetComponent<Transform>();

		dirty |= m_Dirty;
		if (dirty)
		{
			t->SetWorld(t->Combine(parentWorld));
			m_Dirty = false;
		}

		if (m_GO)
		{
			// TODO: Render Queue
			//Renderer::AddToRenderQueue(m_GO);

			auto transform = m_GO->GetComponent<Transform>();
			auto mesh = m_GO->GetComponent<TestMesh>();
			auto shader = m_GO->GetComponent<TestMaterial>()->GetShader();

			shader->Bind();
			shader->SetMat4("projection", projection);
			shader->SetMat4("view", view);
			shader->SetMat4("model", transform->Combine());

			Renderer::Render(mesh->VAO, shader);
		}

		for (auto child : m_Childeren)
		{
			child->Render(projection, view, *t, dirty);
		}
	}

	void GraphNode::SetDirty(bool dirty)
	{
		m_Dirty = dirty;
	}

	void GraphNode::AddChild(GraphNode* child)
	{
		m_Childeren.push_back(child);
	}

	std::vector<GraphNode*> GraphNode::GetChildren()
	{
		return m_Childeren;
	}
}
