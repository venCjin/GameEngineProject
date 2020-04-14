#pragma once

#include "Buffers.h"
#include "Shader.h"
#include "Renderer/VertexArray.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>

#include <map>
//#include "Matrix4f.h"

using namespace std;
#define ToRadian(x) (float)(((x) * M_PI / 180.0f))
#define ToDegree(x) (float)(((x) * 180.0f / M_PI))
#define ZERO_MEM(a) memset(a, 0, sizeof(a))
#define ZERO_MEM_VAR(var) memset(&var, 0, sizeof(var))
#define SAFE_DELETE(p) if (p) { delete p; p = NULL; }

namespace sixengine {

	typedef unsigned int uint;
	#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))


	struct Texture {
		unsigned int id;
		std::string type;
		std::string path;
	};

	class Mesh
	{
	public:

		Mesh();

		~Mesh();

		bool LoadMesh(const string& Filename, bool WithAdjacencies);

		void Render();

		uint NumBones() const
		{
			return m_NumBones;
		}

		void BoneTransform(float TimeInSeconds, vector<glm::mat4>& Transforms);

	private:
//#define NUM_BONES_PER_VEREX 4

		struct BoneInfo
		{
			glm::mat4 BoneOffset = glm::mat4(0.0f);
			glm::mat4 FinalTransformation = glm::mat4(0.0f);;

			/*BoneInfo()
			{
				BoneOffset.SetZero();
				FinalTransformation.SetZero();
			}*/
		};

		/*struct VertexBoneData
		{
			uint IDs[NUM_BONES_PER_VEREX] = { 0 };
			float Weights[NUM_BONES_PER_VEREX] = { 0 };

			VertexBoneData()
			{
				Reset();
			};

			void Reset()
			{
				ZERO_MEM(IDs);
				ZERO_MEM(Weights);
			}

			void AddBoneData(uint BoneID, float Weight);
		};*/

		void CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
		void CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
		void CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
		uint FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim);
		uint FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
		uint FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim);
		const aiNodeAnim* FindNodeAnim(const aiAnimation* pAnimation, const string NodeName);
		void ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode, const glm::mat4& ParentTransform);
		bool InitFromScene(const aiScene* pScene, const string& Filename);
		void InitMesh(uint MeshIndex,
			const aiMesh* paiMesh,
			vector<Vertex>& Vertices,			
			vector<unsigned int>& Indices);
		void LoadBones(uint MeshIndex, const aiMesh* paiMesh, vector<Vertex>& vertices);
		//bool InitMaterials(const aiScene* pScene, const string& Filename);
		void Clear();

#define INVALID_MATERIAL 0xFFFFFFFF

		enum VB_TYPES {
			INDEX_BUFFER,
			POS_VB,
			NORMAL_VB,
			TEXCOORD_VB,
			BONE_VB,
			NUM_VBs
		};

		VertexArray* VAO;
		//GLuint m_VAO;
		//GLuint m_Buffers[NUM_VBs];
		//unsigned int VBO, EBO;
		struct MeshEntry {
			MeshEntry()
			{
				NumIndices = 0;
				BaseVertex = 0;
				BaseIndex = 0;
				MaterialIndex = INVALID_MATERIAL;
			}

			unsigned int NumIndices;
			unsigned int BaseVertex;
			unsigned int BaseIndex;
			unsigned int MaterialIndex;
		};

		vector<MeshEntry> m_Entries;
		//vector<Texture*> m_Textures;

		map<string, uint> m_BoneMapping; // maps a bone name to its index
		uint m_NumBones;
		vector<BoneInfo> m_BoneInfo;
		glm::mat4 m_GlobalInverseTransform;


		bool m_withAdjacencies;

		const aiScene* m_pScene;
		Assimp::Importer m_Importer;
	};

}