#include "pch.h"
#include "PrimitiveUtils.h"
#include "Renderer/Gizmo.h"

namespace sixengine {

	std::vector<GizmoVertex> PrimitiveUtils::generateBoxVertices(float x, float y, float z)
	{
		x = x / 2.0f;
		y = y / 2.0f;
		z = z / 2.0f;
		std::vector<GizmoVertex> vertices{
			// top face
			GizmoVertex{ glm::vec3{-x,  y, -z}/*, glm::vec3{ 0.0f,  1.0f,  0.0f}, glm::vec2{ 0.0f,  0.0f}*/ },	// bottom left
			GizmoVertex{ glm::vec3{ x,  y, -z}/*, glm::vec3{ 0.0f,  1.0f,  0.0f}, glm::vec2{ 1.0f,  0.0f}*/ },	// bottom right
			GizmoVertex{ glm::vec3{-x,  y,  z}/*, glm::vec3{ 0.0f,  1.0f,  0.0f}, glm::vec2{ 0.0f,  1.0f}*/ },	// top left
			GizmoVertex{ glm::vec3{ x,  y,  z}/*, glm::vec3{ 0.0f,  1.0f,  0.0f}, glm::vec2{ 1.0f,  1.0f}*/ },	// top right
			// bottom face
			GizmoVertex{ glm::vec3{ x, -y, -z}/*, glm::vec3{ 0.0f, -1.0f,  0.0f}, glm::vec2{ 0.0f,  0.0f}*/ },	// bottom left
			GizmoVertex{ glm::vec3{-x, -y, -z}/*, glm::vec3{ 0.0f, -1.0f,  0.0f}, glm::vec2{ 1.0f,  0.0f}*/ },	// bottom right
			GizmoVertex{ glm::vec3{ x, -y,  z}/*, glm::vec3{ 0.0f, -1.0f,  0.0f}, glm::vec2{ 0.0f,  1.0f}*/ },	// top left
			GizmoVertex{ glm::vec3{-x, -y,  z}/*, glm::vec3{ 0.0f, -1.0f,  0.0f}, glm::vec2{ 1.0f,  1.0f}*/ },	// top right
			// front face
			GizmoVertex{ glm::vec3{ x, -y,  z}/*, glm::vec3{ 0.0f,  0.0f,  1.0f}, glm::vec2{ 0.0f,  0.0f}*/ },	// bottom left
			GizmoVertex{ glm::vec3{-x, -y,  z}/*, glm::vec3{ 0.0f,  0.0f,  1.0f}, glm::vec2{ 1.0f,  0.0f}*/ },	// bottom right
			GizmoVertex{ glm::vec3{ x,  y,  z}/*, glm::vec3{ 0.0f,  0.0f,  1.0f}, glm::vec2{ 0.0f,  1.0f}*/ },	// top left
			GizmoVertex{ glm::vec3{-x,  y,  z}/*, glm::vec3{ 0.0f,  0.0f,  1.0f}, glm::vec2{ 1.0f,  1.0f}*/ },	// top right
			// back face
			GizmoVertex{ glm::vec3{-x, -y, -z}/*, glm::vec3{ 0.0f,  0.0f, -1.0f}, glm::vec2{ 0.0f,  0.0f}*/ },	// bottom left
			GizmoVertex{ glm::vec3{ x, -y, -z}/*, glm::vec3{ 0.0f,  0.0f, -1.0f}, glm::vec2{ 1.0f,  0.0f}*/ },	// bottom right
			GizmoVertex{ glm::vec3{-x,  y, -z}/*, glm::vec3{ 0.0f,  0.0f, -1.0f}, glm::vec2{ 0.0f,  1.0f}*/ },	// top left
			GizmoVertex{ glm::vec3{ x,  y, -z}/*, glm::vec3{ 0.0f,  0.0f, -1.0f}, glm::vec2{ 1.0f,  1.0f}*/ },	// top right
			// right face
			GizmoVertex{ glm::vec3{ x, -y, -z}/*, glm::vec3{ 1.0f,  0.0f,  0.0f}, glm::vec2{ 0.0f,  0.0f}*/ },	// bottom left
			GizmoVertex{ glm::vec3{ x, -y,  z}/*, glm::vec3{ 1.0f,  0.0f,  0.0f}, glm::vec2{ 1.0f,  0.0f}*/ },	// bottom right
			GizmoVertex{ glm::vec3{ x,  y, -z}/*, glm::vec3{ 1.0f,  0.0f,  0.0f}, glm::vec2{ 0.0f,  1.0f}*/ },	// top left
			GizmoVertex{ glm::vec3{ x,  y,  z}/*, glm::vec3{ 1.0f,  0.0f,  0.0f}, glm::vec2{ 1.0f,  1.0f}*/ },	// top right
			// left face
			GizmoVertex{ glm::vec3{-x, -y,  z}/*, glm::vec3{-1.0f,  0.0f,  0.0f}, glm::vec2{ 0.0f,  0.0f}*/ },	// bottom left
			GizmoVertex{ glm::vec3{-x, -y, -z}/*, glm::vec3{-1.0f,  0.0f,  0.0f}, glm::vec2{ 1.0f,  0.0f}*/ },	// bottom right
			GizmoVertex{ glm::vec3{-x,  y,  z}/*, glm::vec3{-1.0f,  0.0f,  0.0f}, glm::vec2{ 0.0f,  1.0f}*/ },	// top left
			GizmoVertex{ glm::vec3{-x,  y, -z}/*, glm::vec3{-1.0f,  0.0f,  0.0f}, glm::vec2{ 1.0f,  1.0f}*/ },	// top right
		};

		return vertices;
	}

	std::vector<unsigned int> PrimitiveUtils::generateBoxIndices()
	{
		std::vector <unsigned int> indices{
			0, 1, 2,
			1, 3, 2,
			4, 6, 5,
			5, 6, 7,
			8, 9, 10,
			9, 11, 10,
			12, 14, 13,
			13, 14, 15,
			16, 17, 18,
			17, 19, 18,
			20, 22, 21,
			21, 22, 23			
		}; 
		return indices;
	}

	std::vector<GizmoVertex> PrimitiveUtils::generateSphereVertices(int sectorCount, int stackCount, float radius)
	{
		std::vector<GizmoVertex> vertices;
		float x, y, z, xz;                              // GizmoVertex position
		//float nx, ny, nz, lengthInv = 1.0f / radius;    // GizmoVertex normal
		//float s, t;                                     // GizmoVertex texCoord

		float sectorStep = 2 * glm::pi<float>() / sectorCount;
		float stackStep = glm::pi<float>() / stackCount;
		float sectorAngle, stackAngle;

		for (int i = 0; i <= stackCount; ++i)
		{
			stackAngle = glm::pi<float>() / 2 - i * stackStep;        // starting from pi/2 to -pi/2
			xz = radius * glm::cos(stackAngle);             // r * cos(u)
			y = radius * glm::sin(stackAngle);              // r * sin(u)

			// add (sectorCount+1) vertices per stack
			// the first and last vertices have same position and normal, but different tex coords
			for (int j = 0; j <= sectorCount; ++j)
			{
				//LOG_CORE_INFO(j);
				sectorAngle = j * sectorStep;           // starting from 0 to 2pi

				// GizmoVertex position (x, y, z)
				x = xz * glm::cos(sectorAngle);             // r * cos(u) * cos(v)
				z = xz * glm::sin(sectorAngle);             // r * cos(u) * sin(v)
				vertices.push_back(GizmoVertex{ glm::vec3{ x, y, z } });

				// normalized GizmoVertex normal (nx, ny, nz)
				//nx = x * lengthInv;
				//ny = y * lengthInv;
				//nz = z * lengthInv;
				//normals.push_back(nx);
				//normals.push_back(ny);
				//normals.push_back(nz);

				// GizmoVertex tex coord (s, t) range between [0, 1]
				//s = (float)j / sectorCount;
				//t = (float)i / stackCount;
				//texCoords.push_back(s);
				//texCoords.push_back(t);
			}
			
		}
		return vertices;
	}


	std::vector<unsigned int> PrimitiveUtils::generateSphereIndices(int sectorCount, int stackCount)
	{
		std::vector<unsigned int> indices;
		int k1, k2;
		for (int i = 0; i < stackCount; ++i)
		{
			k1 = i * (sectorCount + 1);     // beginning of current stack
			k2 = k1 + sectorCount + 1;      // beginning of next stack

			for (int j = 0; j < sectorCount; ++j, ++k1, ++k2)
			{
				// 2 triangles per sector excluding first and last stacks
				// k1 => k2 => k1+1
				if (i != 0)
				{
					indices.push_back(k1);
					indices.push_back(k2);
					indices.push_back(k1 + 1);
				}

				// k1+1 => k2 => k2+1
				if (i != (stackCount - 1))
				{
					indices.push_back(k1 + 1);
					indices.push_back(k2);
					indices.push_back(k2 + 1);
				}
			}
		}
		return indices;
	}

	std::vector<GizmoVertex> PrimitiveUtils::generateConeVertices(int sectorCount, float radius, float height)
	{
		std::vector<GizmoVertex> vertices;
		float sectorStep = 2 * glm::pi<float>() / sectorCount;

		__debugbreak(); //TODO: implement

		return vertices;
	}

	std::vector<unsigned int> PrimitiveUtils::generateConeIndices(int sectorCount)
	{
		std::vector<unsigned int> indices;
		
		__debugbreak(); //TODO: implemnet

		return indices;
	}

	std::vector<GizmoVertex> PrimitiveUtils::generateCapsuleVertices(int sectorCount, int stackCount, float radius, float height)
	{
		std::vector<GizmoVertex> vertices;
		float x, y, z, xz;                              // GizmoVertex position
		//float nx, ny, nz, lengthInv = 1.0f / radius;    // GizmoVertex normal
		//float s, t;                                     // GizmoVertex texCoord

		float sectorStep = 2 * glm::pi<float>() / sectorCount;
		float stackStep = glm::pi<float>() / stackCount;
		float sectorAngle, stackAngle;

		for (int i = 0; i <= stackCount; ++i)
		{
			stackAngle = glm::pi<float>() / 2 - i * stackStep;        // starting from pi/2 to -pi/2
			xz = radius * glm::cos(stackAngle);             // r * cos(u)
			y = radius * glm::sin(stackAngle);              // r * sin(u)
			if (stackAngle > 0)
			{
				y += height / 2.0f;
			}
			else if(stackAngle < 0)
			{
				y -= height / 2.0f;
			}
			// add (sectorCount+1) vertices per stack
			// the first and last vertices have same position and normal, but different tex coords
			for (int j = 0; j <= sectorCount; ++j)
			{
				//LOG_CORE_INFO(j);
				sectorAngle = j * sectorStep;           // starting from 0 to 2pi

				// GizmoVertex position (x, y, z)
				x = xz * glm::cos(sectorAngle);             // r * cos(u) * cos(v)
				z = xz * glm::sin(sectorAngle);             // r * cos(u) * sin(v)
				
				vertices.push_back(GizmoVertex{ glm::vec3{ x, y , z } });

				// normalized GizmoVertex normal (nx, ny, nz)
				//nx = x * lengthInv;
				//ny = y * lengthInv;
				//nz = z * lengthInv;
				//normals.push_back(nx);
				//normals.push_back(ny);
				//normals.push_back(nz);

				// GizmoVertex tex coord (s, t) range between [0, 1]
				//s = (float)j / sectorCount;
				//t = (float)i / stackCount;
				//texCoords.push_back(s);
				//texCoords.push_back(t);
			}

		}
		return vertices;
	}

	std::vector<unsigned int> PrimitiveUtils::generateCapsuleIndices(int sectorCount, int stackCount)
	{
		std::vector<unsigned int> indices;
		int k1, k2;
		for (int i = 0; i < stackCount; ++i)
		{
			k1 = i * (sectorCount + 1);     // beginning of current stack
			k2 = k1 + sectorCount + 1;      // beginning of next stack

			for (int j = 0; j < sectorCount; ++j, ++k1, ++k2)
			{
				// 2 triangles per sector excluding first and last stacks
				// k1 => k2 => k1+1
				if (i != 0)
				{
					indices.push_back(k1);
					indices.push_back(k2);
					indices.push_back(k1 + 1);
				}

				// k1+1 => k2 => k2+1
				if (i != (stackCount - 1))
				{
					indices.push_back(k1 + 1);
					indices.push_back(k2);
					indices.push_back(k2 + 1);
				}
			}
		}
		return indices;
	}

	std::vector<GizmoVertex> PrimitiveUtils::generateQuadVertices()
	{
		std::vector<GizmoVertex> vertices{
		
			GizmoVertex{ glm::vec3{ 0.5f, -0.5f,  0.0f}/*, glm::vec3{ 0.0f,  0.0f,  1.0f}, glm::vec2{ 0.0f,  0.0f} */},	// bottom left
			GizmoVertex{ glm::vec3{-0.5f, -0.5f,  0.0f}/*, glm::vec3{ 0.0f,  0.0f,  1.0f}, glm::vec2{ 1.0f,  0.0f} */},	// bottom right
			GizmoVertex{ glm::vec3{ 0.5f,  0.5f,  0.0f}/*, glm::vec3{ 0.0f,  0.0f,  1.0f}, glm::vec2{ 0.0f,  1.0f} */},	// top left
			GizmoVertex{ glm::vec3{-0.5f,  0.5f,  0.0f}/*, glm::vec3{ 0.0f,  0.0f,  1.0f}, glm::vec2{ 1.0f,  1.0f} */},	// top right
		};

		return vertices;
	}

	std::vector<unsigned int> PrimitiveUtils::generateQuadIndices()
	{
		std::vector <unsigned int> indices{
				0, 1, 2,
				1, 3, 2
		};
		return indices;
	}



	void PrimitiveUtils::GenerateQuad(std::vector<GizmoVertex>& vertices, std::vector<unsigned int>& indices)
	{
		vertices = PrimitiveUtils::generateQuadVertices();
		indices = PrimitiveUtils::generateQuadIndices();
	}

	void PrimitiveUtils::GenerateBox(std::vector<GizmoVertex>& vertices, std::vector<unsigned int>& indices, float x, float y, float z)
	{
		vertices = PrimitiveUtils::generateBoxVertices(x, y, z);
		indices = PrimitiveUtils::generateBoxIndices();
	}

	void PrimitiveUtils::GenerateSphere(std::vector<GizmoVertex>& vertices, std::vector<unsigned int>& indices, float radius, int sectorCount, int stackCount)
	{
		vertices = PrimitiveUtils::generateSphereVertices(sectorCount, stackCount, radius);
		indices = PrimitiveUtils::generateSphereIndices(sectorCount, stackCount);
	}

	void PrimitiveUtils::GenerateCone(std::vector<GizmoVertex>& vertices, std::vector<unsigned int>& indices, float radius, float height, int sectorCount)
	{
		vertices = PrimitiveUtils::generateConeVertices(sectorCount, radius, height);
		indices = PrimitiveUtils::generateConeIndices(sectorCount);
	}

	void PrimitiveUtils::GenerateCapsule(std::vector<GizmoVertex>& vertices, std::vector<unsigned int>& indices, float radius, float height, int sectorCount, int stackCount)
	{
		vertices = PrimitiveUtils::generateCapsuleVertices(sectorCount, stackCount, radius, height);
		indices = PrimitiveUtils::generateCapsuleIndices(sectorCount, stackCount);
	}

}
