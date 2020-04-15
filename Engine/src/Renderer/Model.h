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

using namespace std;
//#define ToRadian(x) (float)(((x) * M_PI / 180.0f))
//#define ToDegree(x) (float)(((x) * 180.0f / M_PI))
//#define ZERO_MEM(a) memset(a, 0, sizeof(a))
//#define ZERO_MEM_VAR(var) memset(&var, 0, sizeof(var))
#define SAFE_DELETE(p) if (p) { delete p; p = NULL; }

namespace sixengine {


	typedef unsigned int uint;
#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))

	unsigned int TextureFromFile(const char *path, const string &directory, bool gamma = false);

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

		bool LoadModel(const string& Filename);

		void Render(Shader* shader);

		uint NumBones() const
		{
			return m_NumBones;
		}

		void BoneTransform(float TimeInSeconds, vector<glm::mat4>& Transforms);

	private:

		struct BoneInfo
		{
			glm::mat4 BoneOffset = glm::mat4(0.0f);
			glm::mat4 FinalTransformation = glm::mat4(0.0f);;

		};


		void CalcInterpolatedScaling(aiVector3D& out, float animationTime, const aiNodeAnim* nodeAnim);
		void CalcInterpolatedRotation(aiQuaternion& out, float animationTime, const aiNodeAnim* nodeAnim);
		void CalcInterpolatedPosition(aiVector3D& out, float animationTime, const aiNodeAnim* nodeAnim);
		uint FindScaling(float animationTime, const aiNodeAnim* nodeAnim);
		uint FindRotation(float animationTime, const aiNodeAnim* nodeAnim);
		uint FindPosition(float animationTime, const aiNodeAnim* nodeAnim);
		const aiNodeAnim* FindNodeAnim(const aiAnimation* animation, const string nodeName);
		void ReadNodeHierarchy(float animationTime, const aiNode* node, const glm::mat4& parentTransform);
		bool InitFromScene(const aiScene* scene, const string& filename);
		void InitMesh(uint MeshIndex, const aiMesh* mesh, vector<Vertex>& vertices, vector<unsigned int>& indices);
		void LoadBones(uint MeshIndex, const aiMesh* mesh, vector<Vertex>& vertices);
		vector<Texture> LoadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName);
		//bool InitMaterials(const aiScene* pScene, const string& Filename);
		void Clear();

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

			vector<Texture> Textures;
		};

		vector<MeshEntry> m_Entries;
		//vector<Texture*> m_Textures;

		map<string, uint> m_BoneMapping; // maps a bone name to its index
		uint m_NumBones;
		vector<BoneInfo> m_BoneInfo;
		glm::mat4 m_GlobalInverseTransform;
		vector<Texture> m_Textures;
		vector<Texture> m_TexturesLoaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
		string m_Directory;

		const aiScene* m_Scene;
		Assimp::Importer m_Importer;
	};
}