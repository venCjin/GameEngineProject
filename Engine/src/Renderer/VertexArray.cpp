#include "pch.h"
#include "VertexArray.h"

namespace sixengine {

	GLenum VertexArray::ConvertToOpenGLDataType(VertexDataType type)
	{
		switch (type)
		{
			case VertexDataType::INT:			return GL_INT;
			case VertexDataType::FLOAT:			return GL_FLOAT;
			case VertexDataType::VEC2I:			return GL_INT;
			case VertexDataType::VEC3I:			return GL_INT;
			case VertexDataType::VEC4I:			return GL_INT;
			case VertexDataType::VEC2F:			return GL_FLOAT;
			case VertexDataType::VEC3F:			return GL_FLOAT;
			case VertexDataType::VEC4F:			return GL_FLOAT;
			case VertexDataType::MAT3:			return GL_FLOAT;
			case VertexDataType::MAT4:			return GL_FLOAT;
			case VertexDataType::BOOL:			return GL_BOOL;
		}

		CORE_ASSERT(0, "COULDN'T CONVERT TO OGL DATA TYPE!");
		return 0;
	}


	VertexArray::VertexArray()
	{
		glCreateVertexArrays(1, &m_ID);
	}

	VertexArray::~VertexArray()
	{
		if (m_IBuffer != nullptr)
			m_IBuffer->~IndexBuffer();

		for (auto element : m_VBuffers_List)
			element->~VertexBuffer();

		m_VBuffers_List.clear();
		glDeleteVertexArrays(1, &m_ID);
	}

	void VertexArray::Bind() const
	{
		glBindVertexArray(m_ID);
	}

	void VertexArray::UnBind() const
	{
		glBindVertexArray(0);
	}

	void VertexArray::AddVertexBuffer(VertexBuffer& vBuffer)
	{
		glBindVertexArray(m_ID);
		vBuffer.Bind();

		if (vBuffer.GetLayout().GetLayoutElements().size() > 0) {

			unsigned int i = 0;
			for (auto& element : vBuffer.GetLayout().GetLayoutElements())
			{
				glVertexAttribPointer(
					i, 
					element.VertexDataTypeCount(element.VertexDataType), 
					ConvertToOpenGLDataType(element.VertexDataType),
					(element.Normalized ? true : false), 
					vBuffer.GetLayout().GetLayoutStride(), 
					(const void*)element.Offset
				);

				glEnableVertexAttribArray(i);
				i++;
			}

			m_VBuffers_List.push_back(&vBuffer);
		}
	}

	void VertexArray::AddIndexBuffer(IndexBuffer& iBuffer)
	{
		glBindVertexArray(m_ID);
		iBuffer.Bind();
		m_IBuffer = &iBuffer;
	}

}
