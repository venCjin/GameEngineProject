#pragma once

#include <pch.h>
#include "GameObject.h"
#include "Gameplay/GameObject.h"
#include "Gameplay/Components/Transform.h"
#include "AI/NavMesh/NavAgent.h"
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
			child->Render(Transform(this), dirty);
		}
	}

	void GameObject::OnDrawGizmos(bool dirty)
	{
		auto transform = GetComponent<Transform>();
		float time = Timer::Instance()->GetTime(SECOND);
		std::vector<Gizmo*> survived;
		
		for (auto child : m_Gizmos)
		{
			if (time > child->endTime)
			{
				child->~Gizmo();
			}
			else
			{
				survived.push_back(child);
			}
		}
		m_Gizmos.clear();
		m_Gizmos = survived;

		for (auto child : m_Gizmos)
		{
			child->Draw(transform->GetModelMatrix());
		}

		for (auto child : m_Childeren)
		{
			child->OnDrawGizmos(m_Dirty);
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

#ifdef DEBUG
	void GameObject::ImGuiWriteSceneTree()
	{
		for each (auto & child in m_Childeren)
		{
			child->ImGuiWriteSceneTreeNode();
		}
	}

	void GameObject::ImGuiWriteSceneTreeNode()
	{
		if (ImGui::TreeNode((void*)(intptr_t)m_Entity.GetID().GetID(), "Child %d", m_Entity.GetID().GetID()))
		{
			ImGui::Text("Transform");
			
			glm::vec3 pos = GetComponent<Transform>()->GetLocalPosition();
			if (ImGui::DragFloat3("Position", pos.data.data, 0.1f))
			{
				GetComponent<Transform>()->SetLocalPosition(pos);
			}

			//TODO: rotation is likely to go crazy
			glm::vec3 rot = GetComponent<Transform>()->GetLocalOrientation();
			if (ImGui::DragFloat3("Rotation", rot.data.data, 1.0f))
			{
				GetComponent<Transform>()->SetLocalOrientation(rot);
			}

			glm::vec3 scale = GetComponent<Transform>()->GetLocalScale();
			if (ImGui::DragFloat3("Scale", scale.data.data, 0.1f, 0.001f, 100.0f))
			{
				GetComponent<Transform>()->SetLocalScale(scale);
			}

			///
			if (HasComponent<NavAgent>())
			{
				ImGui::InputFloat3("Nav agent destination", GetComponent<NavAgent>()->m_Destination.data.data);
				ImGui::Checkbox("move", &GetComponent<NavAgent>()->m_ProcedeMoving);
			}
			///

			ImGui::Text("Children");
			for each (auto& child in m_Childeren)
			{
				child->ImGuiWriteSceneTreeNode();
			}

			ImGui::TreePop();
		}
	}

	void GameObject::ImGuiWriteUITree()
	{
		for each (auto & child in m_Childeren)
		{
			child->ImGuiWriteUITreeNode();
		}
	}

	void GameObject::ImGuiWriteUITreeNode()
	{
		if (ImGui::TreeNode((void*)(intptr_t)m_Entity.GetID().GetID(), "Child %d", m_Entity.GetID().GetID()))
		{
			ImGui::Text("Transform");

			glm::vec3 pos = GetComponent<Transform>()->GetLocalPosition();
			if (ImGui::DragFloat3("Position", pos.data.data))
			{
				GetComponent<Transform>()->SetLocalPosition(pos);
			}

			ImGui::Text("Children");
			for each (auto & child in m_Childeren)
			{
				child->ImGuiWriteUITreeNode();
			}

			ImGui::TreePop();
		}
	}
#endif // DEBUG


}