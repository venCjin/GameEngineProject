#pragma once
#include "pch.h"
#include "Renderer/Buffers.h"

namespace sixengine {

	class PrimitiveUtils
	{
	private:
		static std::vector<Vertex> generateCubeVertices();
		static std::vector<unsigned int> generateCubeIndices();
		static std::vector<Vertex> generateSphereVertices(int sectorCount, int stackCount, float radius);
		static std::vector<unsigned int> generateSphereIndices(int sectorCount, int stackCount);
		static std::vector<Vertex> generateCapsuleVertices(int sectorCount, int stackCount, float radius, float heights);
		static std::vector<unsigned int> generateCapsuleIndices(int sectorCount, int stackCount);

	public:
		static void GenerateCube(std::vector<Vertex>& vertices, std::vector<unsigned int>& indieces);
		static void GenerateSphere(std::vector<Vertex>& vertices, std::vector<unsigned int>& indieces, int sectorCount=10, int stackCount=10, float radius = .5f);
		static void GenerateCapsule(std::vector<Vertex>& vertices, std::vector<unsigned int>& indieces, int sectorCount = 10, int stackCount = 10, float radius = .5f, float heights = .5f);
	};

}
