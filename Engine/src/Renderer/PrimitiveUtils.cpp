#include "pch.h"
#include "PrimitiveUtils.h"


namespace sixengine {


	std::vector<Vertex> PrimitiveUtils::generateCubeVertices()
	{


		std::vector<Vertex> vertices{
				Vertex{ 0.5f,  0.5f, 0.5f}, // top right
				Vertex{ 0.5f, -0.5f, 0.5f}, // bottom right
				Vertex{-0.5f, -0.5f, 0.5f}, // bottom left
				Vertex{-0.5f,  0.5f, 0.5f},  // top left 
				Vertex{ 0.5f,  0.5f, -0.5f}, // top right
				Vertex{ 0.5f, -0.5f, -0.5f}, // bottom right
				Vertex{-0.5f, -0.5f, -0.5f}, // bottom left
				Vertex{-0.5f,  0.5f, -0.5f}  // top left 
		};

		return vertices;
	}

	std::vector<unsigned int> PrimitiveUtils::generateCubeIndieces()
	{

		std::vector <unsigned int> indices{
			0, 1, 3,
			1, 2, 3,
			4, 5, 7,
			5, 6, 7,
			0, 4, 5,
			0, 1, 5,
			7, 4, 0,
			4, 3, 0,
			7, 3, 2,
			7, 6, 2,
			2, 1, 5,
			5, 2, 6
		};

 
		return indices;
	}

	std::vector<Vertex> PrimitiveUtils::generateSphereVertices(int sectorCount, int stackCount, float radius)
	{
		std::vector<Vertex> vertices;
		LOG_CORE_INFO(sectorCount);
		LOG_CORE_INFO(stackCount);
		float x, y, z, xy;                              // vertex position
		//float nx, ny, nz, lengthInv = 1.0f / radius;    // vertex normal
		//float s, t;                                     // vertex texCoord

		float sectorStep = 2 * glm::pi<float>() / sectorCount;
		float stackStep = glm::pi<float>() / stackCount;
		float sectorAngle, stackAngle;

		for (int i = 0; i <= stackCount; ++i)
		{
			stackAngle = glm::pi<float>() / 2 - i * stackStep;        // starting from pi/2 to -pi/2
			xy = radius * glm::cos(stackAngle);             // r * cos(u)
			z = radius * glm::sin(stackAngle);              // r * sin(u)

			// add (sectorCount+1) vertices per stack
			// the first and last vertices have same position and normal, but different tex coords
			for (int j = 0; j <= sectorCount; ++j)
			{
				LOG_CORE_INFO(j);
				sectorAngle = j * sectorStep;           // starting from 0 to 2pi

				// vertex position (x, y, z)
				x = xy * glm::cos(sectorAngle);             // r * cos(u) * cos(v)
				y = xy * glm::sin(sectorAngle);             // r * cos(u) * sin(v)
				vertices.push_back(Vertex{ x, y, z });

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


	std::vector<unsigned int> PrimitiveUtils::generateSphereIndieces(int sectorCount, int stackCount)
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

	void PrimitiveUtils::GenerateCube(std::vector<Vertex>& vertices, std::vector<unsigned int>& indieces)
	{

		vertices = PrimitiveUtils::generateCubeVertices();
		indieces = PrimitiveUtils::generateCubeIndieces();
	}

	void PrimitiveUtils::GenerateSphere(std::vector<Vertex>& vertices, std::vector<unsigned int>& indieces, int sectorCount, int stackCount, float radius)
	{
		//LOG_CORE_WARN(stackCount);
		vertices = PrimitiveUtils::generateSphereVertices(sectorCount, stackCount, radius);
		indieces = PrimitiveUtils::generateSphereIndieces(sectorCount, stackCount);

	}

}
