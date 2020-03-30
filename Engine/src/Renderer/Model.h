#pragma once
#include "Mesh.h"
#include "Shader.h"
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace sixengine {
	
	unsigned int TextureFromFile(const char *path, const std::string &directory, bool gamma = false);

	class Model
	{
	public:
		void Draw(Shader* shader);

		Model(char *path)
		{
			LoadModel(path);
		}		
		~Model();

	private:
		std::vector<Mesh> m_Meshes;
		std::vector<Texture> m_TexturesLoaded;
		std::string m_Directory;

		void LoadModel(std::string path);
		void ProcessNode(aiNode *node, const aiScene *scene);
		Mesh ProcessMesh(aiMesh *mesh, const aiScene *scene);
		std::vector<Texture> LoadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
	};
}
