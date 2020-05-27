#pragma once

#include <pch.h>
#include "GameObject.h"
#include "Gameplay/GameObject.h"
#include "Gameplay/Components/Transform.h"
#include "Renderer/Material.h"
#include "Renderer/Model.h"
#include "Renderer/Renderer.h"
#include "Renderer/BatchRenderer.h"
#include "Renderer/Gizmo.h"
#include "Core/Timer.h"

namespace sixengine {

	void GameObject::Render(bool first)
	{
		for (auto child : m_Childeren)
		{
			child->Render(Transform(this), first);
		}
	}

	void GameObject::Render(Transform parentWorld, bool dirty)
	{
		auto transform = GetComponent<Transform>();

		BatchRenderer::Instance()->SubmitCommand(this, transform->GetModelMatrix());

		for (auto child : m_Childeren)
		{
			child->Render();
		}
	}

	void GameObject::OnDrawGizmos(bool first)
	{
		for (auto child : m_Childeren)
		{
			child->OnDrawGizmos(Transform(this), first);
		}
	}

	void GameObject::OnDrawGizmos(Transform parentWorld, bool dirty)
	{
		auto transform = GetComponent<Transform>();

		for (auto gizmo : m_Gizmos)
		{
			gizmo->Draw(transform->GetModelMatrix());
		}
		for (auto child : m_Childeren)
		{
			child->OnDrawGizmos(Transform(this), m_Dirty);
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

	void GameObject::AddGizmo(Gizmo* gizmo)
	{
		m_Gizmos.push_back(gizmo);
	}

	std::vector<GameObject*> GameObject::GetChildren()
	{
		return m_Childeren;
	}

}