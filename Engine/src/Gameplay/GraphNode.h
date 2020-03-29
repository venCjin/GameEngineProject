#pragma once

#include <vector>
#include "Gameplay\GameObject.h"
//#include "Gameplay\Components\Transform.h"
#include "Gameplay\Components\TestMaterial.h"

namespace sixengine {

	class Transform;

	class GraphNode
	{
	private:
		bool m_Dirty;
		GameObject* m_GO;
		std::vector<GraphNode*> m_Childeren;

	public:
		GraphNode(GameObject* go);

		void Render(glm::mat4 projection, glm::mat4 view);

		void Render(glm::mat4 projection, glm::mat4 view, Transform parentWorld, bool dirty);

		void SetDirty(bool dirty);

		void AddChild(GraphNode* child);

		std::vector<GraphNode*> GetChildren();
	};

}
