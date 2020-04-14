#pragma once
#include "Renderer/Buffers.h"

typedef unsigned int GLenum;

namespace sixengine {

	class VertexArray {

	public:

		VertexArray();
		~VertexArray();

		void Bind() const;
		void UnBind() const;

		void AddVertexBuffer(VertexBuffer& vBuffer);
		void AddIndexBuffer(IndexBuffer& iBuffer);

		const std::vector<VertexBuffer*>& GetVertexBuffers() const { return m_VBuffers_List; }
		IndexBuffer* GetIndexBuffer() const { return m_IBuffer; }
		
		
	private:

		GLenum ConvertToOpenGLDataType(VertexDataType type);

		unsigned int m_ID;
		IndexBuffer* m_IBuffer = nullptr;
		std::vector<VertexBuffer*> m_VBuffers_List;

	};

}
