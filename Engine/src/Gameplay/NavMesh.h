#pragma once

#include "DetourNavMesh.h"
#include "DetourNavMeshQuery.h"

namespace sixengine {

	static const int NAVMESHSET_MAGIC = 'M' << 24 | 'S' << 16 | 'E' << 8 | 'T'; //'MSET';
	static const int NAVMESHSET_VERSION = 1;

	struct NavMeshSetHeader
	{
		int magic;
		int version;
		int numTiles;
		dtNavMeshParams params;
	};

	struct NavMeshTileHeader
	{
		dtTileRef tileRef;
		int dataSize;
	};

	class NavMesh
	{
	private:
		class dtNavMesh* m_navMesh = nullptr;
		class dtNavMeshQuery* m_navQuery = nullptr;

		float m_agentHeight = 2.0f;
		float m_agentRadius = 1.0f;
		float m_agentMaxClimb = 0.5f;
	public:
		NavMesh();

		void Init(const char* path);

		dtNavMesh* Load(const char* path);

		class dtNavMesh* getNavMesh() { return m_navMesh; }
		class dtNavMeshQuery* getNavMeshQuery() { return m_navQuery; }
		float getAgentRadius() { return m_agentRadius; }
		float getAgentHeight() { return m_agentHeight; }
		float getAgentClimb() { return m_agentMaxClimb; }

	#ifdef DEBUG
		void drawMesh();
		void drawAgent(const glm::vec3 pos);
	#endif // DEBUG

	};

}