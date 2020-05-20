#include "pch.h"
#include "Model.h"
#include <glad/glad.h> 
#include "stb_image.h"

#define POSITION_LOCATION    0
#define TEX_COORD_LOCATION   1
#define NORMAL_LOCATION      2
#define BONE_ID_LOCATION     3
#define BONE_WEIGHT_LOCATION 4

namespace sixengine {

	Model::Model()
	{
		VAO = nullptr;
		m_NumBones = 0;
		m_Scene = NULL;
	}

	Model::Model(const std::string& filename, unsigned ID)
	{
		m_ID = ID;
		VAO = nullptr;
		m_NumBones = 0;
		m_Scene = NULL;
		LoadModel(filename);
	}

	Model::~Model()
	{
		Clear();
	}


	void Model::Clear()
	{		
		if (VAO != nullptr)
		{
			VAO->~VertexArray();
			VAO = nullptr;
		}
	}

	void Model::LoadAnimationNodes()
	{
		const aiAnimation* animation = m_Scene->mAnimations[0];

		for (uint i = 0; i < animation->mNumChannels; i++) {
			const aiNodeAnim* nodeAnim = animation->mChannels[i];

			if (m_NodeAnimationMapping.find(std::string(nodeAnim->mNodeName.data)) == m_NodeAnimationMapping.end())
			{
				m_NodeAnimationMapping[std::string(nodeAnim->mNodeName.data)] = nodeAnim;
			}			
		}
	}


	bool Model::LoadModel(const std::string& filename)
	{
		// Release the previously loaded mesh (if it exists)
		Clear();
		
		bool ret = false;

		m_Scene = m_Importer.ReadFile(filename.c_str(), aiProcess_Triangulate |
			aiProcess_GenSmoothNormals |
			aiProcess_FlipUVs |
			aiProcess_JoinIdenticalVertices);
		if (m_Scene) {
			m_GlobalInverseTransform = glm::transpose(glm::make_mat4(&m_Scene->mRootNode->mTransformation.a1));
			m_GlobalInverseTransform = glm::inverse(m_GlobalInverseTransform);
			
			m_Directory = filename.substr(0, filename.find_last_of('/'));
			ret = InitFromScene(m_Scene, filename);
		}
		else {
			//printf("Error parsing '%s': '%s'\n", filename.c_str(), m_Importer.GetErrorString());
			LOG_ERROR("Error parsing {0}: {1}", filename.c_str(), m_Importer.GetErrorString());
		}

		// Make sure the VAO is not changed from the outside
		//glBindVertexArray(0);
		if (m_Scene->HasAnimations())
		{
			LoadAnimationNodes();
		}
		

		return ret;
	}


	bool Model::InitFromScene(const aiScene* scene, const std::string& filename)
	{
		m_Entries.resize(scene->mNumMeshes);
		//m_Textures.resize(pScene->mNumMaterials);

		uint numVertices = 0;
		m_TotalNumIndices = 0;

		uint verticesPerPrim = 3;

		// Count the number of vertices and indices
		for (uint i = 0; i < m_Entries.size(); i++) {
			m_Entries[i].MaterialIndex = scene->mMeshes[i]->mMaterialIndex;
			m_Entries[i].NumIndices = scene->mMeshes[i]->mNumFaces * verticesPerPrim;
			m_Entries[i].BaseVertex = numVertices;
			m_Entries[i].BaseIndex = m_TotalNumIndices;

			numVertices += scene->mMeshes[i]->mNumVertices;
			m_TotalNumIndices += m_Entries[i].NumIndices;

		}

		// Reserve space in the vectors for the vertex attributes and indices
		vertices.reserve(numVertices);
		indices.reserve(m_TotalNumIndices);

		// Initialize the meshes in the scene one by one
		for (uint i = 0; i < m_Entries.size(); i++) {
			const aiMesh* mesh = scene->mMeshes[i];
			InitMesh(i, mesh, vertices, indices);
		}

		/*if (!InitMaterials(pScene, filename)) {
			return false;
		}*/

		
		VAO = new VertexArray();			
		VertexBuffer* vbo = new VertexBuffer(&vertices[0], vertices.size());		
		IndexBuffer* ebo = new IndexBuffer(&indices[0], indices.size());
	
		vbo->SetLayout({
			{ VertexDataType::VEC3F, "position" },
			{ VertexDataType::VEC3F, "normal" },
			{ VertexDataType::VEC2F, "texCoords" },
			{ VertexDataType::VEC4I, "boneIDs" },
			{ VertexDataType::VEC4F, "boneWeights" }
		});

		VAO->AddVertexBuffer(*vbo);
		VAO->AddIndexBuffer(*ebo);

		//glBindVertexArray(0);

		return true;
	}

	void Model::InitMesh(uint meshIndex, const aiMesh* mesh, std::vector<Vertex>& vertices, std::vector<uint>& indices)
	{
		std::vector<Texture> textures;


		// Populate the vertex attribute vectors
		for (uint i = 0; i < mesh->mNumVertices; i++) {

			Vertex vertex;
			glm::vec3 vector;

			// Postion
			vector = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
			vertex.Position = vector;

			// Normals
			if (mesh->mNormals)
				vector = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };
			else
				vector = glm::vec3(0.0f);
			vertex.Normal = vector;

			// TexCoords
			if (mesh->mTextureCoords[0])
			{
				glm::vec2 vec{ mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
				vertex.TexCoords = vec;
			}
			else
				vertex.TexCoords = glm::vec2(0.0f, 0.0f);

			vertices.push_back(vertex);
		}

		LoadBones(meshIndex, mesh, vertices);

		for (uint i = 0; i < mesh->mNumFaces; i++) {
			const aiFace& Face = mesh->mFaces[i];
			indices.push_back(Face.mIndices[0]);
			indices.push_back(Face.mIndices[1]);
			indices.push_back(Face.mIndices[2]);
		}
		
		// Materials
		aiMaterial* material = m_Scene->mMaterials[mesh->mMaterialIndex];
		std::vector<Texture> diffuseMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		// 2. specular maps
		std::vector<Texture> specularMaps = LoadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		// 3. normal maps
		std::vector<Texture> normalMaps = LoadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
		// 4. height maps
		std::vector<Texture> heightMaps = LoadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");

		textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

		m_Entries[meshIndex].Textures = textures;
	}


	void Model::LoadBones(uint meshIndex, const aiMesh* mesh, std::vector<Vertex>& vertices)
	{
		for (uint i = 0; i < mesh->mNumBones; i++) {
			uint boneIndex = 0;
			std::string boneName(mesh->mBones[i]->mName.data);

			if (m_BoneMapping.find(boneName) == m_BoneMapping.end()) {
				// Allocate an index for a new bone
				boneIndex = m_NumBones;
				m_NumBones++;
				BoneInfo bi;
				m_BoneInfo.push_back(bi);
				m_BoneInfo[boneIndex].BoneOffset = glm::transpose(glm::make_mat4(&mesh->mBones[i]->mOffsetMatrix.a1));
				m_BoneMapping[boneName] = boneIndex;
			}
			else {
				boneIndex = m_BoneMapping[boneName];
			}

			for (uint j = 0; j < mesh->mBones[i]->mNumWeights; j++) {
				uint VertexID = m_Entries[meshIndex].BaseVertex + mesh->mBones[i]->mWeights[j].mVertexId;
				float Weight = mesh->mBones[i]->mWeights[j].mWeight;
				vertices[VertexID].AddBoneData(boneIndex, Weight);
			}
		}
	}

	std::vector<Texture> Model::LoadMaterialTextures(aiMaterial * mat, aiTextureType type, std::string typeName)
	{
		std::vector<Texture> textures;
		for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
		{
			aiString str;
			mat->GetTexture(type, i, &str);
			// check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
			bool skip = false;
			for (unsigned int j = 0; j < m_TexturesLoaded.size(); j++)
			{
				if (std::strcmp(m_TexturesLoaded[j].path.data(), str.C_Str()) == 0)
				{
					textures.push_back(m_TexturesLoaded[j]);
					skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
					break;
				}
			}
			if (!skip)
			{   // if texture hasn't been loaded already, load it
				Texture texture;
				texture.id = TextureFromFile(str.C_Str(), this->m_Directory);
				texture.type = typeName;
				texture.path = str.C_Str();
				textures.push_back(texture);
				m_TexturesLoaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
			}
		}
		return textures;
	}


	/*bool Mesh::InitMaterials(const aiScene* pScene, const std::string& filename)
	{
		// Extract the directory part from the file name
		std::string::size_type SlashIndex = filename.find_last_of("/");
		std::string Dir;

		if (SlashIndex == std::string::npos) {
			Dir = ".";
		}
		else if (SlashIndex == 0) {
			Dir = "/";
		}
		else {
			Dir = filename.substr(0, SlashIndex);
		}

		bool Ret = true;

		// Initialize the materials
		for (uint i = 0; i < pScene->mNumMaterials; i++) {
			const aiMaterial* pMaterial = pScene->mMaterials[i];

			m_Textures[i] = NULL;

			if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
				aiString Path;

				if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
					std::string p(Path.data);

					if (p.substr(0, 2) == ".\\") {
						p = p.substr(2, p.size() - 2);
					}

					std::string FullPath = Dir + "/" + p;

					m_Textures[i] = new Texture(GL_TEXTURE_2D, FullPath.c_str());

					if (!m_Textures[i]->Load()) {
						printf("Error loading texture '%s'\n", FullPath.c_str());
						delete m_Textures[i];
						m_Textures[i] = NULL;
						Ret = false;
					}
					else {
						printf("%d - loaded texture '%s'\n", i, FullPath.c_str());
					}
				}
			}
		}

		return Ret;
	}
	*/

	void Model::Render(Shader* shader)
	{
		unsigned int diffuseNr = 1;
		unsigned int specularNr = 1;
		unsigned int normalNr = 1;
		unsigned int heightNr = 1;


		VAO->Bind();
		//glDrawElements(GL_TRIANGLES, VAO->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, 0);

		/*glDrawElements(GL_TRIANGLES, VAO->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, 0);*/
		//glBindVertexArray(m_VAO);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		for (uint i = 0; i < m_Entries.size(); i++)
		{


			/*const uint MaterialIndex = m_Entries[i].MaterialIndex;

			if (m_Textures[MaterialIndex]) {
				m_Textures[MaterialIndex]->Bind(COLOR_TEXTURE_UNIT);
			}*/
			for (unsigned int j = 0; j < m_Entries[i].Textures.size(); j++)
			{
				glActiveTexture(GL_TEXTURE0 + i* m_Entries.size() + j); // active proper texture unit before binding
				// retrieve texture number (the N in diffuse_textureN)
				std::string number;
				std::string name = m_Entries[i].Textures[j].type;
				if (name == "texture_diffuse")
					number = std::to_string(diffuseNr++);
				else if (name == "texture_specular")
					number = std::to_string(specularNr++); // transfer unsigned int to stream
				else if (name == "texture_normal")
					number = std::to_string(normalNr++); // transfer unsigned int to stream
				else if (name == "texture_height")
					number = std::to_string(heightNr++); // transfer unsigned int to stream

				// now set the sampler to the correct texture unit
				glUniform1i(glGetUniformLocation(shader->GetID(), (name + number).c_str()), i* m_Entries.size() + j);
				// and finally bind the texture
				glBindTexture(GL_TEXTURE_2D, m_Entries[i].Textures[j].id);
			}
			glDrawElementsBaseVertex(GL_TRIANGLES,
				m_Entries[i].NumIndices,
				GL_UNSIGNED_INT,
				(void*)(sizeof(uint) * m_Entries[i].BaseIndex),
				m_Entries[i].BaseVertex);
		}
		//glDrawElements(GL_TRIANGLES, VAO->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, 0);

		VAO->Unbind();

		// Make sure the VAO is not changed from the outside    
		//glBindVertexArray(0);
	}


	uint Model::FindPosition(float animationTime, const aiNodeAnim* nodeAnim)
	{
		for (uint i = 0; i < nodeAnim->mNumPositionKeys - 1; i++) {
			if (animationTime < (float)nodeAnim->mPositionKeys[i + 1].mTime) {
				return i;
			}
		}

		//assert(0);

		return 0;
	}


	uint Model::FindRotation(float animationTime, const aiNodeAnim* nodeAnim)
	{
		//assert(nodeAnim->mNumRotationKeys > 0);

		for (uint i = 0; i < nodeAnim->mNumRotationKeys - 1; i++) {
			if (animationTime < (float)nodeAnim->mRotationKeys[i + 1].mTime) {
				return i;
			}
		}

		//assert(0);

		return 0;
	}


	uint Model::FindScaling(float animationTime, const aiNodeAnim* nodeAnim)
	{
		assert(nodeAnim->mNumScalingKeys > 0);

		for (uint i = 0; i < nodeAnim->mNumScalingKeys - 1; i++) {
			if (animationTime < (float)nodeAnim->mScalingKeys[i + 1].mTime) {
				return i;
			}
		}

		assert(0);

		return 0;
	}


	void Model::CalcInterpolatedPosition(aiVector3D& out, float animationTime, const aiNodeAnim* nodeAnim)
	{
		if (nodeAnim->mNumPositionKeys == 1) {
			out = nodeAnim->mPositionKeys[0].mValue;
			return;
		}

		uint positionIndex = FindPosition(animationTime, nodeAnim);
		uint nextpositionIndex = (positionIndex + 1);
		assert(nextpositionIndex < nodeAnim->mNumPositionKeys);
		float deltaTime = (float)(nodeAnim->mPositionKeys[nextpositionIndex].mTime - nodeAnim->mPositionKeys[positionIndex].mTime);
		float factor = (animationTime - (float)nodeAnim->mPositionKeys[positionIndex].mTime) / deltaTime;
		//assert(Factor >= 0.0f && Factor <= 1.0f);
		const aiVector3D& start = nodeAnim->mPositionKeys[positionIndex].mValue;
		const aiVector3D& end = nodeAnim->mPositionKeys[nextpositionIndex].mValue;
		aiVector3D delta = end - start;
		out = start + factor * delta;
	}

	glm::mat4 Model::CalculateInterpolatedTransform(float animationTime, const aiNodeAnim * nodeAnim)
	{
		uint positionIndex = FindPosition(animationTime, nodeAnim);
		uint nextpositionIndex = (positionIndex + 1);

		return glm::mat4();
	}


	void Model::CalcInterpolatedRotation(aiQuaternion& out, float animationTime, const aiNodeAnim* nodeAnim)
	{
		// we need at least two values to interpolate...
		if (nodeAnim->mNumRotationKeys == 1) {
			out = nodeAnim->mRotationKeys[0].mValue;
			return;
		}

		uint rotationIndex = FindRotation(animationTime, nodeAnim);
		uint nextRotationIndex = (rotationIndex + 1);
		assert(nextRotationIndex < nodeAnim->mNumRotationKeys);
		float deltaTime = (float)(nodeAnim->mRotationKeys[nextRotationIndex].mTime - nodeAnim->mRotationKeys[rotationIndex].mTime);
		float factor = (animationTime - (float)nodeAnim->mRotationKeys[rotationIndex].mTime) / deltaTime;
		//assert(Factor >= 0.0f && Factor <= 1.0f);
		const aiQuaternion& startRotationQ = nodeAnim->mRotationKeys[rotationIndex].mValue;
		const aiQuaternion& endRotationQ = nodeAnim->mRotationKeys[nextRotationIndex].mValue;
		aiQuaternion::Interpolate(out, startRotationQ, endRotationQ, factor);
		out = out.Normalize();
	}


	void Model::CalcInterpolatedScaling(aiVector3D& out, float animationTime, const aiNodeAnim* nodeAnim)
	{
		if (nodeAnim->mNumScalingKeys == 1) {
			out = nodeAnim->mScalingKeys[0].mValue;
			return;
		}

		uint scalingIndex = FindScaling(animationTime, nodeAnim);
		uint nextScalingIndex = (scalingIndex + 1);
		assert(nextScalingIndex < nodeAnim->mNumScalingKeys);
		float deltaTime = (float)(nodeAnim->mScalingKeys[nextScalingIndex].mTime - nodeAnim->mScalingKeys[scalingIndex].mTime);
		float factor = (animationTime - (float)nodeAnim->mScalingKeys[scalingIndex].mTime) / deltaTime;
		//assert(Factor >= 0.0f && Factor <= 1.0f);
		const aiVector3D& start = nodeAnim->mScalingKeys[scalingIndex].mValue;
		const aiVector3D& end = nodeAnim->mScalingKeys[nextScalingIndex].mValue;
		aiVector3D delta = end - start;
		out = start + factor * delta;
	}


	void Model::ReadNodeHierarchy(float animationTime, const aiNode* node, const glm::mat4& parentTransform)
	{
		std::string nodeName(node->mName.data);

		const aiAnimation* animation = m_Scene->mAnimations[0];

		glm::mat4 nodeTransformation(glm::transpose(glm::make_mat4(&node->mTransformation.a1)));

		const aiNodeAnim* nodeAnim = m_NodeAnimationMapping[nodeName]; /*FindNodeAnim(animation, nodeName);*/

		if (nodeAnim) {
			// Interpolate scaling and generate scaling transformation matrix
			aiVector3D scaling;
			CalcInterpolatedScaling(scaling, animationTime, nodeAnim);
			glm::mat4 scalingM = glm::mat4(1.0f);
			scalingM = glm::scale(scalingM, glm::vec3(scaling.x, scaling.y, scaling.z));

			// Interpolate rotation and generate rotation transformation matrix
			aiQuaternion rotationQ;
			CalcInterpolatedRotation(rotationQ, animationTime, nodeAnim);
			aiMatrix3x3 a = rotationQ.GetMatrix();
			glm::mat4 rotationM = glm::transpose(glm::make_mat3(&a.a1));

			// Interpolate translation and generate translation transformation matrix
			aiVector3D translation;
			CalcInterpolatedPosition(translation, animationTime, nodeAnim);
			glm::mat4 translationM = glm::mat4(1.0f);
			translationM = glm::translate(translationM, glm::vec3(translation.x, translation.y, translation.z));

			// Combine the above transformations
			nodeTransformation = translationM * rotationM * scalingM;
		}

		glm::mat4 globalTransformation = parentTransform * nodeTransformation;

		if (m_BoneMapping.find(nodeName) != m_BoneMapping.end()) {
			uint boneIndex = m_BoneMapping[nodeName];
			m_BoneInfo[boneIndex].FinalTransformation = m_GlobalInverseTransform * globalTransformation * m_BoneInfo[boneIndex].BoneOffset;
		}

		for (uint i = 0; i < node->mNumChildren; i++) {
			ReadNodeHierarchy(animationTime, node->mChildren[i], globalTransformation);
		}
	}


	void Model::BoneTransform(float timeInSeconds, std::vector<glm::mat4>& transforms)
	{
		if (!m_Scene->HasAnimations())
			return;

		float ticksPerSecond = (float)(m_Scene->mAnimations[0]->mTicksPerSecond != 0 ? m_Scene->mAnimations[0]->mTicksPerSecond : 25.0f);
		float timeInTicks = timeInSeconds * ticksPerSecond;
		float animationTime = fmod(timeInTicks, (float)m_Scene->mAnimations[0]->mDuration);

		ReadNodeHierarchy(animationTime, m_Scene->mRootNode, glm::mat4(1.0f));
		transforms.resize(m_NumBones);

		for (uint i = 0; i < m_NumBones; i++) {
			transforms[i] = m_BoneInfo[i].FinalTransformation;
		}
	}

	unsigned int TextureFromFile(const char * path, const std::string & directory, bool gamma)
	{
		std::string filename = std::string(path);
		//std::string fn2 = filename.substr(filename.find_last_of('/'), filename.length());

		filename = directory + "/" + filename;//directory + fn2;
		//LOG_INFO(filename);

		unsigned int textureID;
		glGenTextures(1, &textureID);

		int width, height, nrComponents;
		unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
		if (data)
		{
			GLenum format;
			if (nrComponents == 1)
				format = GL_RED;
			else if (nrComponents == 3)
				format = GL_RGB;
			else if (nrComponents == 4)
				format = GL_RGBA;

			glBindTexture(GL_TEXTURE_2D, textureID);
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			stbi_image_free(data);
		}
		else
		{
			std::string message("Texture failed to load at path: " + std::string(path));
			LOG_WARN(message);
			stbi_image_free(data);
		}

		return textureID;
	}
}
