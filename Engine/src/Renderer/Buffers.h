#pragma once

#include <glad\glad.h>

namespace sixengine {

	struct Vertex;
	enum class VertexDataType
	{
		NONE = 0,
		BOOL,
		INT,
		FLOAT,
		VEC2I, VEC3I, VEC4I,
		VEC2F, VEC3F, VEC4F,
		MAT3, MAT4
	};
	
	struct BufferData
	{
		std::string Name;
		bool Normalized;
		unsigned int Size;
		unsigned int Offset;
		VertexDataType VertexDataType;

		unsigned int VertexDataTypeCount(sixengine::VertexDataType vertexDataType) const; //How many components has each type
		unsigned int VertexDataTypeSize(sixengine::VertexDataType vertexDataType) const; //The size of the data type of the vertex

		BufferData() {}
		BufferData(sixengine::VertexDataType vertexDataType, const std::string& name, bool norm = false)
			: VertexDataType(vertexDataType), Name(name), Normalized(norm), Size(VertexDataTypeSize(vertexDataType)) {}

	};


	class BufferLayout
	{
	public:

		BufferLayout() {}
		BufferLayout(const std::initializer_list<BufferData>& data) : m_BufferLayoutData(data)
		{
			CalculateValues();
		}

		inline const std::vector<BufferData>& GetLayoutElements() const { return m_BufferLayoutData; }
		inline const unsigned int GetLayoutStride() const { return m_BufferLayoutStride; }
		
	private:

		void CalculateValues()
		{
			unsigned int offset = 0;
			m_BufferLayoutStride = 0;
			for (auto& element : m_BufferLayoutData) {

				element.Offset = offset;
				offset += element.Size;
				m_BufferLayoutStride += element.Size;
			}
		}

	private:
		std::vector<BufferData> m_BufferLayoutData;
		unsigned int m_BufferLayoutStride;
	};


	class VertexBuffer
	{
	public:

		VertexBuffer(Vertex* vertices, unsigned int size);
		~VertexBuffer();

		void Bind() const;
		void UnBind() const;

		inline const BufferLayout& GetLayout() const { return m_VBLayout; }
		void SetLayout(const BufferLayout& bufferLayout) { m_VBLayout = bufferLayout; }

	private:
		unsigned int m_ID;
		BufferLayout m_VBLayout;
	};


	class IndexBuffer
	{
	public:

		IndexBuffer(unsigned int* indices, unsigned int count);
		~IndexBuffer();

		void Bind() const;
		void UnBind() const;

		inline const unsigned int GetCount() const { return m_Count; }
	
	private:
		unsigned int m_Count;
		unsigned int m_ID;
	};

}
