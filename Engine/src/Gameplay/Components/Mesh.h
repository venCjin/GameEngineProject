#pragma once

#include <Renderer/Model.h>

namespace sixengine {

	struct Mesh
	{
		Model* m_Model;

		Mesh(Model *model)
			: m_Model(model)
		{
		}

		inline Model* GetModel() const { return m_Model; }
	};

}