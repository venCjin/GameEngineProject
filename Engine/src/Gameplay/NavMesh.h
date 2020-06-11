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
		class dtNavMesh* m_navMesh;
		class dtNavMeshQuery* m_navQuery;

		float m_agentHeight;
		float m_agentRadius;
		float m_agentMaxClimb;
	public:
		NavMesh();

		dtNavMesh* Load(const char* path);

		class dtNavMesh* getNavMesh() { return m_navMesh; }
		class dtNavMeshQuery* getNavMeshQuery() { return m_navQuery; }
		float getAgentRadius() { return m_agentRadius; }
		float getAgentHeight() { return m_agentHeight; }
		float getAgentClimb() { return m_agentMaxClimb; }

	#ifdef DEBUG
		//void ImGuiWriteSceneTree();
		//void ImGuiWriteSceneTreeNode();

		//void ImGuiWriteUITree();
		//void ImGuiWriteUITreeNode();
	#endif // DEBUG

	};

}