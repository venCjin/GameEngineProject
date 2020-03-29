#include "pch.h"
#include "PrimitiveUtils.h"


namespace sixengine {


	std::vector<Vertex> PrimitiveUtils::generateCubeVertices()
	{
		std::vector<Vertex> vertices{
			// top face
			Vertex{ glm::vec3{-0.5f,  0.5f, -0.5f}, glm::vec3{ 0.0f,  1.0f,  0.0f}, glm::vec2{ 0.0f,  0.0f} },	// bottom left
			Vertex{ glm::vec3{ 0.5f,  0.5f, -0.5f}, glm::vec3{ 0.0f,  1.0f,  0.0f}, glm::vec2{ 1.0f,  0.0f} },	// bottom right
			Vertex{ glm::vec3{-0.5f,  0.5f,  0.5f}, glm::vec3{ 0.0f,  1.0f,  0.0f}, glm::vec2{ 0.0f,  1.0f} },	// top left
			Vertex{ glm::vec3{ 0.5f,  0.5f,  0.5f}, glm::vec3{ 0.0f,  1.0f,  0.0f}, glm::vec2{ 1.0f,  1.0f} },	// top right
			// bottom face
			Vertex{ glm::vec3{ 0.5f, -0.5f, -0.5f}, glm::vec3{ 0.0f, -1.0f,  0.0f}, glm::vec2{ 0.0f,  0.0f} },	// bottom left
			Vertex{ glm::vec3{-0.5f, -0.5f, -0.5f}, glm::vec3{ 0.0f, -1.0f,  0.0f}, glm::vec2{ 1.0f,  0.0f} },	// bottom right
			Vertex{ glm::vec3{ 0.5f, -0.5f,  0.5f}, glm::vec3{ 0.0f, -1.0f,  0.0f}, glm::vec2{ 0.0f,  1.0f} },	// top left
			Vertex{ glm::vec3{-0.5f, -0.5f,  0.5f}, glm::vec3{ 0.0f, -1.0f,  0.0f}, glm::vec2{ 1.0f,  1.0f} },	// top right
			// front face
			Vertex{ glm::vec3{ 0.5f, -0.5f,  0.5f}, glm::vec3{ 0.0f,  0.0f,  1.0f}, glm::vec2{ 0.0f,  0.0f} },	// bottom left
			Vertex{ glm::vec3{-0.5f, -0.5f,  0.5f}, glm::vec3{ 0.0f,  0.0f,  1.0f}, glm::vec2{ 1.0f,  0.0f} },	// bottom right
			Vertex{ glm::vec3{ 0.5f,  0.5f,  0.5f}, glm::vec3{ 0.0f,  0.0f,  1.0f}, glm::vec2{ 0.0f,  1.0f} },	// top left
			Vertex{ glm::vec3{-0.5f,  0.5f,  0.5f}, glm::vec3{ 0.0f,  0.0f,  1.0f}, glm::vec2{ 1.0f,  1.0f} },	// top right
			// back face
			Vertex{ glm::vec3{-0.5f, -0.5f, -0.5f}, glm::vec3{ 0.0f,  0.0f, -1.0f}, glm::vec2{ 0.0f,  0.0f} },	// bottom left
			Vertex{ glm::vec3{ 0.5f, -0.5f, -0.5f}, glm::vec3{ 0.0f,  0.0f, -1.0f}, glm::vec2{ 1.0f,  0.0f} },	// bottom right
			Vertex{ glm::vec3{-0.5f,  0.5f, -0.5f}, glm::vec3{ 0.0f,  0.0f, -1.0f}, glm::vec2{ 0.0f,  1.0f} },	// top left
			Vertex{ glm::vec3{ 0.5f,  0.5f, -0.5f}, glm::vec3{ 0.0f,  0.0f, -1.0f}, glm::vec2{ 1.0f,  1.0f} },	// top right
			// right face
			Vertex{ glm::vec3{ 0.5f, -0.5f, -0.5f}, glm::vec3{ 1.0f,  0.0f,  0.0f}, glm::vec2{ 0.0f,  0.0f} },	// bottom left
			Vertex{ glm::vec3{ 0.5f, -0.5f,  0.5f}, glm::vec3{ 1.0f,  0.0f,  0.0f}, glm::vec2{ 1.0f,  0.0f} },	// bottom right
			Vertex{ glm::vec3{ 0.5f,  0.5f, -0.5f}, glm::vec3{ 1.0f,  0.0f,  0.0f}, glm::vec2{ 0.0f,  1.0f} },	// top left
			Vertex{ glm::vec3{ 0.5f,  0.5f,  0.5f}, glm::vec3{ 1.0f,  0.0f,  0.0f}, glm::vec2{ 1.0f,  1.0f} },	// top right
			// left face
			Vertex{ glm::vec3{-0.5f, -0.5f,  0.5f}, glm::vec3{-1.0f,  0.0f,  0.0f}, glm::vec2{ 0.0f,  0.0f} },	// bottom left
			Vertex{ glm::vec3{-0.5f, -0.5f, -0.5f}, glm::vec3{-1.0f,  0.0f,  0.0f}, glm::vec2{ 1.0f,  0.0f} },	// bottom right
			Vertex{ glm::vec3{-0.5f,  0.5f,  0.5f}, glm::vec3{-1.0f,  0.0f,  0.0f}, glm::vec2{ 0.0f,  1.0f} },	// top left
			Vertex{ glm::vec3{-0.5f,  0.5f, -0.5f}, glm::vec3{-1.0f,  0.0f,  0.0f}, glm::vec2{ 1.0f,  1.0f} },	// top right
		};

		return vertices;
	}

	std::vector<unsigned int> PrimitiveUtils::generateCubeIndices()
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

	std::vector<Vertex> PrimitiveUtils::generateSphereVertices(int sectorCount, int stackCount, float radius)
	{
		std::vector<Vertex> vertices;
		float x, y, z, xz;                              // vertex position
		//float nx, ny, nz, lengthInv = 1.0f / radius;    // vertex normal
		//float s, t;                                     // vertex texCoord

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

				// vertex position (x, y, z)
				x = xz * glm::cos(sectorAngle);             // r * cos(u) * cos(v)
				z = xz * glm::sin(sectorAngle);             // r * cos(u) * sin(v)
				vertices.push_back(Vertex{ glm::vec3{ x, y , z } });

				// normalized vertex normal (nx, ny, nz)
				//nx = x * lengthInv;
				//ny = y * lengthInv;
				//nz = z * lengthInv;
				//normals.push_back(nx);
				//normals.push_back(ny);
				//normals.push_back(nz);

				// vertex tex coord (s, t) range between [0, 1]
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

	std::vector<Vertex> PrimitiveUtils::generateCapsuleVertices(int sectorCount, int stackCount, float radius, float height)
	{
		std::vector<Vertex> vertices;
		float x, y, z, xz;                              // vertex position
		//float nx, ny, nz, lengthInv = 1.0f / radius;    // vertex normal
		//float s, t;                                     // vertex texCoord

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

				// vertex position (x, y, z)
				x = xz * glm::cos(sectorAngle);             // r * cos(u) * cos(v)
				z = xz * glm::sin(sectorAngle);             // r * cos(u) * sin(v)
				
				vertices.push_back(Vertex{ glm::vec3{ x, y , z } });

				// normalized vertex normal (nx, ny, nz)
				//nx = x * lengthInv;
				//ny = y * lengthInv;
				//nz = z * lengthInv;
				//normals.push_back(nx);
				//normals.push_back(ny);
				//normals.push_back(nz);

				// vertex tex coord (s, t) range between [0, 1]
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

	void PrimitiveUtils::GenerateCube(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices)
	{

		vertices = PrimitiveUtils::generateCubeVertices();
		indices = PrimitiveUtils::generateCubeIndices();
	}

	void PrimitiveUtils::GenerateSphere(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, int sectorCount, int stackCount, float radius)
	{
		vertices = PrimitiveUtils::generateSphereVertices(sectorCount, stackCount, radius);
		indices = PrimitiveUtils::generateSphereIndices(sectorCount, stackCount);
	}

	void PrimitiveUtils::GenerateCapsule(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, int sectorCount, int stackCount, float radius, float height)
	{
		vertices = PrimitiveUtils::generateCapsuleVertices(sectorCount, stackCount, radius, height);
		indices = PrimitiveUtils::generateCapsuleIndices(sectorCount, stackCount);
	}

}
