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


//#define SAFE_DELETE(p) if (p) { delete p; p = NULL; }

namespace sixengine {


	typedef unsigned int uint;
#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))

	unsigned int TextureFromFile(const char *path, const std::string &directory, bool gamma = false);

	struct Texture {
		unsigned int id;
		std::string type;
		std::string path;
	};

	class Model
	{
	public:

		Model();

		~Model();

		bool LoadModel(const std::string& Filename);

		void Render(Shader* shader);

		uint NumBones() const
		{
			return m_NumBones;
		}

		void BoneTransform(float TimeInSeconds, std::vector<glm::mat4>& Transforms);

	private:

		struct BoneInfo
		{
			glm::mat4 BoneOffset = glm::mat4(0.0f);
			glm::mat4 FinalTransformation = glm::mat4(0.0f);;

		};


		void CalcInterpolatedScaling(aiVector3D& out, float animationTime, const aiNodeAnim* nodeAnim);
		void CalcInterpolatedRotation(aiQuaternion& out, float animationTime, const aiNodeAnim* nodeAnim);
		void CalcInterpolatedPosition(aiVector3D& out, float animationTime, const aiNodeAnim* nodeAnim);
		glm::mat4 CalculateInterpolatedTransform(float animationTime, const aiNodeAnim* nodeAnim);
		uint FindScaling(float animationTime, const aiNodeAnim* nodeAnim);
		uint FindRotation(float animationTime, const aiNodeAnim* nodeAnim);
		uint FindPosition(float animationTime, const aiNodeAnim* nodeAnim);
		void ReadNodeHierarchy(float animationTime, const aiNode* node, const glm::mat4& parentTransform);
		bool InitFromScene(const aiScene* scene, const std::string& filename);
		void InitMesh(uint MeshIndex, const aiMesh* mesh, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices);
		void LoadBones(uint MeshIndex, const aiMesh* mesh, std::vector<Vertex>& vertices);
		std::vector<Texture> LoadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
		//bool InitMaterials(const aiScene* pScene, const std::string& Filename);
		void Clear();
		void LoadAnimationNodes();

#define INVALID_MATERIAL 0xFFFFFFFF

		VertexArray* VAO;

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

			std::vector<Texture> Textures;
		};

		std::vector<MeshEntry> m_Entries;
		//std::vector<Texture*> m_Textures;

		std::map<std::string, uint> m_BoneMapping; // maps a bone name to its index
		std::map<std::string, const aiNodeAnim* > m_NodeAnimationMapping;
		uint m_NumBones;
		std::vector<BoneInfo> m_BoneInfo;
		glm::mat4 m_GlobalInverseTransform;
		std::vector<Texture> m_Textures;
		std::vector<Texture> m_TexturesLoaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
		std::string m_Directory;

		const aiScene* m_Scene;
		Assimp::Importer m_Importer;
	};
}