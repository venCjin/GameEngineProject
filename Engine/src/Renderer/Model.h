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

#include "Core/Profile.h"
namespace sixengine {


	typedef unsigned int uint;

	class Model
	{
	public:

		struct BoneInfo
		{
			glm::mat4 BoneOffset = glm::mat4(0.0f);
			glm::mat4 FinalTransformation = glm::mat4(0.0f);
			std::string Name;
		};

		Model();
		Model(const std::string& filename, unsigned ID);

		~Model();

		bool LoadModel(const std::string& filename);
		bool LoadAnimation(const std::string& filename, std::string name);

		uint NumBones() const
		{
			return m_NumBones;
		}

		void BoneTransform(float currentTimer, float previousTimer, std::vector<glm::mat4>& Transforms, std::string currentAnimationName, std::string previousAnimationName);

		inline std::vector<glm::mat4>& GetCurrentTransforms() { return currentTransforms; }

		inline std::string GetDirectory() { return m_Directory; }
		inline std::string GetName() { return m_Name; }

		unsigned int m_ID;
		VertexArray* VAO;

		std::vector<Vertex> vertices;
		std::vector<uint> indices;
		int m_TotalNumIndices = 0;

		glm::vec3 m_MinAxis;
		glm::vec3 m_MaxAxis;
		std::vector<BoneInfo> m_BoneInfo;
		bool m_FreeBones = false;

	private:
		bool firstRNH = false;	

		void CalcInterpolatedScaling(aiVector3D& out, float animationTime, const aiNodeAnim* nodeAnim);
		void CalcInterpolatedRotation(aiQuaternion& out, float animationTime, const aiNodeAnim* nodeAnim);
		void CalcInterpolatedPosition(aiVector3D& out, float animationTime, const aiNodeAnim* nodeAnim);
		glm::mat4 CalculateInterpolatedTransform(float animationTime, const aiNodeAnim* nodeAnim);
		uint FindScaling(float animationTime, const aiNodeAnim* nodeAnim);
		uint FindRotation(float animationTime, const aiNodeAnim* nodeAnim);
		uint FindPosition(float animationTime, const aiNodeAnim* nodeAnim);
		void ReadNodeHierarchy(float animationTime, const aiNode* node, const glm::mat4& parentTransform, std::string animationName, std::vector<glm::mat4>& transforms);
		bool InitFromScene(const aiScene* scene, const std::string& filename);
		void InitMesh(uint MeshIndex, const aiMesh* mesh/*, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices*/);
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

		struct AnimationEntry
		{
			AnimationEntry()
			{
				scene = nullptr;
				animation = nullptr;
			}

			AnimationEntry(const std::string& filename)
			{
				scene = importer.ReadFile(filename.c_str(), aiProcess_FlipUVs);
				if (scene->HasAnimations())
				{
					animation = scene->mAnimations[0];
					LoadAnimationNodes();
				}

				if (!scene->HasAnimations() || animation == nullptr || nodeAnimationMapping.size() == 0)
					LOG_ERROR("AnimationEntry nullptr");

			}

			void LoadAnimationNodes()
			{			
				for (uint i = 0; i < animation->mNumChannels; i++) {
					const aiNodeAnim* nodeAnim = animation->mChannels[i];

					if (nodeAnimationMapping.find(std::string(nodeAnim->mNodeName.data)) == nodeAnimationMapping.end())
					{
						nodeAnimationMapping[std::string(nodeAnim->mNodeName.data)] = nodeAnim;
					}
				}
			}

			Assimp::Importer importer;
			const aiScene* scene;
			aiAnimation* animation;
			std::map<std::string, const aiNodeAnim* > nodeAnimationMapping;
		};

		std::vector<MeshEntry> m_Entries;

		std::vector<glm::mat4> currentTransforms; // current animation transforms for shader

		std::map<std::string, uint> m_BoneMapping; // maps a bone name to its index
		std::map<std::string, AnimationEntry*> m_AnimationsMapping;
		uint m_NumBones;
		glm::mat4 m_GlobalInverseTransform;
		std::string m_Directory;
		std::string m_Name;

		const aiScene* m_Scene;
		Assimp::Importer m_Importer;
	};
}