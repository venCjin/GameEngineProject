#pragma once
#include "pch.h"
#include "Renderer/Buffers.h"

namespace sixengine {

	struct GizmoVertex;

	class PrimitiveUtils
	{
	private:
		static std::vector<GizmoVertex> generateBoxVertices(float x, float y, float z);
		static std::vector<unsigned int> generateBoxIndices();

		static std::vector<GizmoVertex> generateSphereVertices(int sectorCount, int stackCount, float radius);
		static std::vector<unsigned int> generateSphereIndices(int sectorCount, int stackCount);

		static std::vector<GizmoVertex> generateConeVertices(int sectorCount, float radius, float height);
		static std::vector<unsigned int> generateConeIndices(int sectorCount);

		static std::vector<GizmoVertex> generateCapsuleVertices(int sectorCount, int stackCount, float radius, float heights);
		static std::vector<unsigned int> generateCapsuleIndices(int sectorCount, int stackCount);

		static std::vector<GizmoVertex> generateQuadVertices();
		static std::vector<unsigned int> generateQuadIndices();

	public:
		static void GenerateQuad(std::vector<GizmoVertex>& vertices, std::vector<unsigned int>& indices);
		static void GenerateBox(std::vector<GizmoVertex>& vertices, std::vector<unsigned int>& indices, float x = .5f, float y = .5f, float z = .5f);
		static void GenerateSphere(std::vector<GizmoVertex>& vertices, std::vector<unsigned int>& indices, float radius = .5f, int sectorCount=10, int stackCount=10);
		static void GenerateCone(std::vector<GizmoVertex>& vertices, std::vector<unsigned int>& indices, float radius = .3f, float height = 1.0f, int sectorCount = 10);
		static void GenerateCapsule(std::vector<GizmoVertex>& vertices, std::vector<unsigned int>& indices, float radius = .5f, float heights = .5f, int sectorCount = 10, int stackCount = 10);
	};

}
