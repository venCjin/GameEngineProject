#include "pch.h"
#include "Buffers.h"

#include <glad\glad.h>
#include <GLFW\glfw3.h>


namespace sixengine {

	unsigned int BufferData::VertexDataTypeSize(sixengine::VertexDataType vertexDataType) const
	{
		switch (vertexDataType)
		{
			case VertexDataType::BOOL:	return sizeof(bool);
			case VertexDataType::INT:	return sizeof(int);
			case VertexDataType::VEC2I:	return sizeof(int)   * 2;
			case VertexDataType::VEC3I:	return sizeof(int)   * 3;
			case VertexDataType::VEC4I:	return sizeof(int)   * 4;
			case VertexDataType::FLOAT:	return sizeof(float);
			case VertexDataType::VEC2F:	return sizeof(float) * 2;
			case VertexDataType::VEC3F:	return sizeof(float) * 3;
			case VertexDataType::VEC4F:	return sizeof(float) * 4;
			case VertexDataType::MAT3:	return sizeof(float) * 3 * 3;
			case VertexDataType::MAT4:	return sizeof(float) * 4 * 4;
		}

		CORE_ASSERT(0, "Unknown Vertex Data Type!");
		return 0;
	}

	unsigned int BufferData::VertexDataTypeCount(sixengine::VertexDataType vertexDataType) const
	{
		switch (vertexDataType)
		{
			case VertexDataType::BOOL:	return 1;
			case VertexDataType::INT:	return 1;
			case VertexDataType::VEC2I:	return 2;
			case VertexDataType::VEC3I:	return 3;
			case VertexDataType::VEC4I:	return 4;
			case VertexDataType::FLOAT:	return 1;
			case VertexDataType::VEC2F:	return 2;
			case VertexDataType::VEC3F:	return 3;
			case VertexDataType::VEC4F:	return 4;
			case VertexDataType::MAT3:	return 3 * 3;
			case VertexDataType::MAT4:	return 4 * 4;
		}
		CORE_ASSERT(0, "Unknown Vertex Data Type Count!");
		return 0;
	}

	//-------------- VERTEX BUFFER -------------------------------------------
	VertexBuffer::VertexBuffer(Vertex* vertices, unsigned int size)
	{
		//Setting the buffer to generate (an Array one), the buffer size,
		//the data source (the float* vertices), and for what we are using it
		//(to statically draw)
		glCreateBuffers(1, &m_ID);
		glBindBuffer(GL_ARRAY_BUFFER, m_ID);
		glBufferData(GL_ARRAY_BUFFER, size * sizeof(Vertex), vertices, GL_STATIC_DRAW);
	}

	VertexBuffer::~VertexBuffer()
	{
		glDeleteBuffers(1, &m_ID);
	}

	void VertexBuffer::Bind() const
	{
		//The type of buffer we are binding
		glBindBuffer(GL_ARRAY_BUFFER, m_ID);
	}

	void VertexBuffer::UnBind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}


	//-------------- INDEX BUFFER --------------------------------------------
	IndexBuffer::IndexBuffer(unsigned int* indices, unsigned int count) : m_Count(count)
	{
		glCreateBuffers(1, &m_ID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), indices, GL_STATIC_DRAW);
	}

	IndexBuffer::~IndexBuffer()
	{
		glDeleteBuffers(1, &m_ID);
	}

	void IndexBuffer::Bind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ID);
	}

	void IndexBuffer::UnBind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

}