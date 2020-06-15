#pragma once

#include <pch.h>
#include "NavMesh.h"
#include "DebugDraw.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace sixengine {

	NavMesh::NavMesh()
	{
	}

	void NavMesh::Init(const char* path)
	{
		dtFreeNavMesh(m_navMesh);
		m_navMesh = Load(path);
		m_navQuery = dtAllocNavMeshQuery();
		m_navQuery->init(m_navMesh, 2048);
		LOG_CORE_INFO("NavMesh::Init() : LOADED SUCCESFULLY");
	}

	dtNavMesh* NavMesh::Load(const char* path)
	{
		FILE* fp = fopen(path, "rb");
		if (!fp) return 0;

		// Read header.
		NavMeshSetHeader header;
		size_t readLen = fread(&header, sizeof(NavMeshSetHeader), 1, fp);
		if (readLen != 1)
		{
			fclose(fp);
			return 0;
		}
		if (header.magic != NAVMESHSET_MAGIC)
		{
			fclose(fp);
			return 0;
		}
		if (header.version != NAVMESHSET_VERSION)
		{
			fclose(fp);
			return 0;
		}

		dtNavMesh* mesh = dtAllocNavMesh();
		if (!mesh)
		{
			fclose(fp);
			return 0;
		}
		dtStatus status = mesh->init(&header.params);
		if (dtStatusFailed(status))
		{
			fclose(fp);
			return 0;
		}

		// Read tiles.
		for (int i = 0; i < header.numTiles; ++i)
		{
			NavMeshTileHeader tileHeader;
			readLen = fread(&tileHeader, sizeof(tileHeader), 1, fp);
			if (readLen != 1)
			{
				fclose(fp);
				return 0;
			}

			if (!tileHeader.tileRef || !tileHeader.dataSize)
				break;

			unsigned char* data = (unsigned char*)dtAlloc(tileHeader.dataSize, DT_ALLOC_PERM);
			if (!data) break;
			memset(data, 0, tileHeader.dataSize);
			readLen = fread(data, tileHeader.dataSize, 1, fp);
			if (readLen != 1)
			{
				dtFree(data);
				fclose(fp);
				return 0;
			}

			mesh->addTile(data, tileHeader.dataSize, DT_TILE_FREE_DATA, tileHeader.tileRef, 0);
		}

		fclose(fp);

		return mesh;
	}

#ifdef DEBUG
	void NavMesh::drawNavMesh()
	{

	}

	void NavMesh::drawAgent(const glm::vec3 pos)
	{
		
	}
#endif // DEBUG
}