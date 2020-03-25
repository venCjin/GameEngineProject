#pragma once
#include "pch.h"
#include "Renderer/Buffers.h"
namespace sixengine {
	class PrimitiveUtils
	{
		static std::vector<Vertex> generateCubeVertices();
		static std::vector<unsigned int> generateCubeIndieces();
		static std::vector<Vertex> generateSphereVertices(int sectorCount, int stackCount, float radius);
		static std::vector<unsigned int> generateSphereIndieces(int sectorCount, int stackCount);
	public:
		static void GenerateCube(std::vector<Vertex>& vertices, std::vector<unsigned int>& indieces);
		static void GenerateSphere(std::vector<Vertex>& vertices, std::vector<unsigned int>& indieces, int sectorCount=10, int stackCount=10, float radius = .5f);
		//static void generateCapsule(struct Vertex*, unsigned int*);
	};


}
