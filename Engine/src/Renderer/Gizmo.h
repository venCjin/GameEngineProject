#pragma once

#include "Renderer/VertexArray.h"
#include "Renderer/Shader.h"
#include "Core/ISerializable.h"
#include "Renderer/PrimitiveUtils.h"

#include "glm/glm.hpp"

namespace sixengine {

	class Gizmo : public ISerializable
	{
		VertexArray* m_VAO;
		Shader* m_Shader;
		glm::vec3 m_Color;
	public:
		float endTime;
		bool draw;
		glm::mat4 model;

		Gizmo(VertexArray* vao, Shader* shader, glm::vec3 color, bool enableDraw = true, float timeToLive = 1000000000.0f);
		~Gizmo();

		Gizmo(Shader* shader);
		virtual void Load(std::iostream& stream) override;
		virtual void Save(std::iostream& stream) override;

		static void DrawLine(glm::vec3 start, glm::vec3 end, float timeToLive = 1000000000.0f, glm::vec3 color = { 255, 0, 0 });

		void Draw(const glm::mat4& model);
	};

}

