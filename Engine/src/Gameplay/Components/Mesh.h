#pragma once

#include <Renderer/Model.h>

namespace sixengine {

	struct Mesh : public ISerializable
	{
		Model* m_Model;

		Mesh(Model *model)
			: m_Model(model)
		{
		}

		inline Model* GetModel() const { return m_Model; }

		virtual void Load(std::iostream& stream)
		{
		}

		virtual void Save(std::iostream& stream)
		{
			std::string lineToSave = "-Model " + m_Model->GetDirectory() + "/" + m_Model->GetName();
			//TODO: Animations save
		}
	};

}