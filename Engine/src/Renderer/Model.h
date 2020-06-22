#pragma once
#include <glm/glm.hpp>

#include "Buffers.h"
#include "Shader.h"
#include <Renderer/VertexArray.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>
#include <assimp/Importer.hpp>
#include <map>
#include <limits>
#include <Gameplay/Components/Animation.h>

#include "Core/Profile.h"
namespace sixengine {

	typedef unsigned int uint;

	struct BoneInfo
	{
		glm::mat4 BoneOffset = glm::mat4(0.0f);
		glm::mat4 FinalTransformation = glm::mat4(0.0f);
		glm::mat4 GlobalTransformation = glm::mat4(0.0f);
		std::string Name;
		bool FreeBone = false;
	};

	class Model
	{
	public:

		

		Model();
		Model(const std::string& filename, unsigned ID);

		~Model();

		bool LoadModel(const std::string& filename);

		uint NumBones() const
		{
			return m_NumBones;
		}

		void BoneTransform(AnimationEntry* currentAnimation, AnimationEntry* previousAnimation, std::vector<glm::mat4>& transforms);
		int GetBoneInfoIndex(std::string boneName);
		BoneInfo* GetBoneInfo(int index);

		inline std::vector<glm::mat4>& GetCurrentTransforms() { return currentTransforms; }

		inline std::string GetDirectory() { return m_Directory; }
		inline std::string GetName() { return m_Name; }

		void SetFrustumInfinity();

		unsigned int m_ID;
		VertexArray* VAO;

		std::vector<Vertex> vertices;
		std::vector<uint> indices;
		int m_TotalNumIndices = 0;

		glm::vec3 m_MinAxis;
		glm::vec3 m_MaxAxis;
		std::vector<BoneInfo> m_BoneInfo;
		bool m_FreeBones = false;
		glm::mat4 m_GlobalInverseTransform;
		void LoadGlobalPositions(const aiNode* node, const glm::mat4& parentTransform);
		bool m_GlobalPositionsLoaded = false;
		const aiScene* m_Scene;

	private:
		bool firstRNH = false;	

		void CalcInterpolatedScaling(aiVector3D& out, float animationTime, const aiNodeAnim* nodeAnim);
		void CalcInterpolatedRotation(aiQuaternion& out, float animationTime, const aiNodeAnim* nodeAnim);
		void CalcInterpolatedPosition(aiVector3D& out, float animationTime, const aiNodeAnim* nodeAnim);
		glm::mat4 CalculateInterpolatedTransform(float animationTime, const aiNodeAnim* nodeAnim);
		uint FindScaling(float animationTime, const aiNodeAnim* nodeAnim);
		uint FindRotation(float animationTime, const aiNodeAnim* nodeAnim);
		uint FindPosition(float animationTime, const aiNodeAnim* nodeAnim);
		void ReadNodeHierarchy(float animationTime, const aiNode* node, const glm::mat4& parentTransform, AnimationEntry* animation, std::vector<glm::mat4>& transforms);
		bool InitFromScene(const aiScene* scene, const std::string& filename);
		void InitMesh(uint MeshIndex, const aiMesh* mesh);
		void LoadBones(uint MeshIndex, const aiMesh* mesh, std::vector<Vertex>& vertices);
		void Clear();

		struct MeshEntry 
		{
			MeshEntry()
			{
				NumIndices = 0;
				BaseVertex = 0;
				BaseIndex = 0;
			}

			unsigned int NumIndices;
			unsigned int BaseVertex;
			unsigned int BaseIndex;
		};		

		std::vector<MeshEntry> m_Entries;

		std::vector<glm::mat4> currentTransforms; // current animation transforms for shader

		std::map<std::string, uint> m_BoneMapping; // maps a bone name to its index
		uint m_NumBones;
		std::string m_Directory;
		std::string m_Name;

		Assimp::Importer m_Importer;
	};
}