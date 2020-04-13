#include "pch.h"
#include "Mesh.h"
#include <glad/glad.h> 
#include "stb_image.h"

#define POSITION_LOCATION    0
#define TEX_COORD_LOCATION   1
#define NORMAL_LOCATION      2
#define BONE_ID_LOCATION     3
#define BONE_WEIGHT_LOCATION 4

namespace sixengine {

	Mesh::Mesh()
	{
		m_VAO = 0;
		ZERO_MEM(m_Buffers);
		m_NumBones = 0;
		m_pScene = NULL;
		m_withAdjacencies = false;
	}


	Mesh::~Mesh()
	{
		Clear();
	}


	void Mesh::Clear()
	{
		/*for (uint i = 0; i < m_Textures.size(); i++) {
			SAFE_DELETE(m_Textures[i]);
		}*/

		if (m_Buffers[0] != 0) {
			glDeleteBuffers(ARRAY_SIZE_IN_ELEMENTS(m_Buffers), m_Buffers);
		}

		if (m_VAO != 0) {
			glDeleteVertexArrays(1, &m_VAO);
			m_VAO = 0;
		}
	}


	bool Mesh::LoadMesh(const string& Filename, bool WithAdjacencies)
	{
		m_withAdjacencies = WithAdjacencies;

		// Release the previously loaded mesh (if it exists)
		Clear();

		// Create the VAO
		glGenVertexArrays(1, &m_VAO);
		glBindVertexArray(m_VAO);

		// Create the buffers for the vertices attributes
		glGenBuffers(ARRAY_SIZE_IN_ELEMENTS(m_Buffers), m_Buffers);

		bool Ret = false;

		m_pScene = m_Importer.ReadFile(Filename.c_str(), aiProcess_Triangulate |
			aiProcess_GenSmoothNormals |
			aiProcess_FlipUVs |
			aiProcess_JoinIdenticalVertices);
		if (m_pScene) {
			m_GlobalInverseTransform = glm::transpose(glm::make_mat4(&m_pScene->mRootNode->mTransformation.a1));
			m_GlobalInverseTransform = glm::inverse(m_GlobalInverseTransform);
			
			Ret = InitFromScene(m_pScene, Filename);
		}
		else {
			printf("Error parsing '%s': '%s'\n", Filename.c_str(), m_Importer.GetErrorString());
		}

		// Make sure the VAO is not changed from the outside
		glBindVertexArray(0);

		return Ret;
	}


	bool Mesh::InitFromScene(const aiScene* pScene, const string& Filename)
	{
		m_Entries.resize(pScene->mNumMeshes);
		//m_Textures.resize(pScene->mNumMaterials);

		vector<Vertex> vertices;
		vector<uint> indices;

		uint NumVertices = 0;
		uint NumIndices = 0;

		uint VerticesPerPrim = m_withAdjacencies ? 6 : 3;

		// Count the number of vertices and indices
		for (uint i = 0; i < m_Entries.size(); i++) {
			m_Entries[i].MaterialIndex = pScene->mMeshes[i]->mMaterialIndex;
			m_Entries[i].NumIndices = pScene->mMeshes[i]->mNumFaces * VerticesPerPrim;
			m_Entries[i].BaseVertex = NumVertices;
			m_Entries[i].BaseIndex = NumIndices;

			NumVertices += pScene->mMeshes[i]->mNumVertices;
			NumIndices += m_Entries[i].NumIndices;
		}

		// Reserve space in the vectors for the vertex attributes and indices
		vertices.reserve(NumVertices);
		indices.reserve(NumIndices);

		// Initialize the meshes in the scene one by one
		for (uint i = 0; i < m_Entries.size(); i++) {
			const aiMesh* paiMesh = pScene->mMeshes[i];
			InitMesh(i, paiMesh, vertices/*Positions, Normals, TexCoords, Bones*/, indices);
		}

		/*if (!InitMaterials(pScene, Filename)) {
			return false;
		}*/

		// Generate and populate the buffers with vertex attributes and the indices


		
		/// LEARN OPENGL
		glGenVertexArrays(1, &m_VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(m_VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
			&indices[0], GL_STATIC_DRAW);

		// vertex positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		// vertex normals
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
		// vertex texture coords
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

		glEnableVertexAttribArray(3);
		glVertexAttribIPointer(3, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, IDs));

		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Weights));

		glBindVertexArray(0);

		
		return true;
	}

	void Mesh::InitMesh(uint MeshIndex,
		const aiMesh* mesh,
		vector<Vertex>& vertices,
		vector<uint>& indices)
	{
		const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

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

		LoadBones(MeshIndex, mesh, vertices);

		/*if (m_withAdjacencies) {
			FindAdjacencies(paiMesh, Indices);
		}
		else*/ {
			// Populate the index buffer
			for (uint i = 0; i < mesh->mNumFaces; i++) {
				const aiFace& Face = mesh->mFaces[i];
				assert(Face.mNumIndices == 3);
				indices.push_back(Face.mIndices[0]);
				indices.push_back(Face.mIndices[1]);
				indices.push_back(Face.mIndices[2]);
			}
		}
	}


	void Mesh::LoadBones(uint MeshIndex, const aiMesh* pMesh, vector<Vertex>& vertices)
	{
		for (uint i = 0; i < pMesh->mNumBones; i++) {
			uint BoneIndex = 0;
			string BoneName(pMesh->mBones[i]->mName.data);

			if (m_BoneMapping.find(BoneName) == m_BoneMapping.end()) {
				// Allocate an index for a new bone
				BoneIndex = m_NumBones;
				m_NumBones++;
				BoneInfo bi;
				m_BoneInfo.push_back(bi);
				m_BoneInfo[BoneIndex].BoneOffset = glm::transpose(glm::make_mat4(&pMesh->mBones[i]->mOffsetMatrix.a1));
				m_BoneMapping[BoneName] = BoneIndex;
			}
			else {
				BoneIndex = m_BoneMapping[BoneName];
			}

			for (uint j = 0; j < pMesh->mBones[i]->mNumWeights; j++) {
				uint VertexID = m_Entries[MeshIndex].BaseVertex + pMesh->mBones[i]->mWeights[j].mVertexId;
				float Weight = pMesh->mBones[i]->mWeights[j].mWeight;
				vertices[VertexID].AddBoneData(BoneIndex, Weight);
			}
		}
	}


	/*bool Mesh::InitMaterials(const aiScene* pScene, const string& Filename)
	{
		// Extract the directory part from the file name
		string::size_type SlashIndex = Filename.find_last_of("/");
		string Dir;

		if (SlashIndex == string::npos) {
			Dir = ".";
		}
		else if (SlashIndex == 0) {
			Dir = "/";
		}
		else {
			Dir = Filename.substr(0, SlashIndex);
		}

		bool Ret = true;

		// Initialize the materials
		for (uint i = 0; i < pScene->mNumMaterials; i++) {
			const aiMaterial* pMaterial = pScene->mMaterials[i];

			m_Textures[i] = NULL;

			if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
				aiString Path;

				if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
					string p(Path.data);

					if (p.substr(0, 2) == ".\\") {
						p = p.substr(2, p.size() - 2);
					}

					string FullPath = Dir + "/" + p;

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
	}*/


	void Mesh::Render()
	{
		/*VAO->Bind();
		glDrawElements(GL_TRIANGLES, VAO->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, 0);*/


		glBindVertexArray(m_VAO);

		uint Topology = m_withAdjacencies ? GL_TRIANGLES_ADJACENCY : GL_TRIANGLES;

		for (uint i = 0; i < m_Entries.size(); i++) {
			const uint MaterialIndex = m_Entries[i].MaterialIndex;

			//assert(MaterialIndex < m_Textures.size());

			/*if (m_Textures[MaterialIndex]) {
				m_Textures[MaterialIndex]->Bind(COLOR_TEXTURE_UNIT);
			}*/

			glDrawElementsBaseVertex(Topology,
				m_Entries[i].NumIndices,
				GL_UNSIGNED_INT,
				(void*)(sizeof(uint) * m_Entries[i].BaseIndex),
				m_Entries[i].BaseVertex);
		}

		// Make sure the VAO is not changed from the outside    
		glBindVertexArray(0);
	}


	uint Mesh::FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim)
	{
		for (uint i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++) {
			if (AnimationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime) {
				return i;
			}
		}

		assert(0);

		return 0;
	}


	uint Mesh::FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim)
	{
		assert(pNodeAnim->mNumRotationKeys > 0);

		for (uint i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++) {
			if (AnimationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime) {
				return i;
			}
		}

		assert(0);

		return 0;
	}


	uint Mesh::FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim)
	{
		assert(pNodeAnim->mNumScalingKeys > 0);

		for (uint i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++) {
			if (AnimationTime < (float)pNodeAnim->mScalingKeys[i + 1].mTime) {
				return i;
			}
		}

		assert(0);

		return 0;
	}


	void Mesh::CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
	{
		if (pNodeAnim->mNumPositionKeys == 1) {
			Out = pNodeAnim->mPositionKeys[0].mValue;
			return;
		}

		uint PositionIndex = FindPosition(AnimationTime, pNodeAnim);
		uint NextPositionIndex = (PositionIndex + 1);
		assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);
		float DeltaTime = (float)(pNodeAnim->mPositionKeys[NextPositionIndex].mTime - pNodeAnim->mPositionKeys[PositionIndex].mTime);
		float Factor = (AnimationTime - (float)pNodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;
		//assert(Factor >= 0.0f && Factor <= 1.0f);
		const aiVector3D& Start = pNodeAnim->mPositionKeys[PositionIndex].mValue;
		const aiVector3D& End = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
		aiVector3D Delta = End - Start;
		Out = Start + Factor * Delta;
	}


	void Mesh::CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
	{
		// we need at least two values to interpolate...
		if (pNodeAnim->mNumRotationKeys == 1) {
			Out = pNodeAnim->mRotationKeys[0].mValue;
			return;
		}

		uint RotationIndex = FindRotation(AnimationTime, pNodeAnim);
		uint NextRotationIndex = (RotationIndex + 1);
		assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
		float DeltaTime = (float)(pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[RotationIndex].mTime);
		float Factor = (AnimationTime - (float)pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
		//assert(Factor >= 0.0f && Factor <= 1.0f);
		const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
		const aiQuaternion& EndRotationQ = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
		aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);
		Out = Out.Normalize();
	}


	void Mesh::CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
	{
		if (pNodeAnim->mNumScalingKeys == 1) {
			Out = pNodeAnim->mScalingKeys[0].mValue;
			return;
		}

		uint ScalingIndex = FindScaling(AnimationTime, pNodeAnim);
		uint NextScalingIndex = (ScalingIndex + 1);
		assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
		float DeltaTime = (float)(pNodeAnim->mScalingKeys[NextScalingIndex].mTime - pNodeAnim->mScalingKeys[ScalingIndex].mTime);
		float Factor = (AnimationTime - (float)pNodeAnim->mScalingKeys[ScalingIndex].mTime) / DeltaTime;
		//assert(Factor >= 0.0f && Factor <= 1.0f);
		const aiVector3D& Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
		const aiVector3D& End = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
		aiVector3D Delta = End - Start;
		Out = Start + Factor * Delta;
	}


	void Mesh::ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode, const glm::mat4& ParentTransform)
	{
		string NodeName(pNode->mName.data);

		const aiAnimation* pAnimation = m_pScene->mAnimations[0];

		glm::mat4 NodeTransformation(glm::transpose(glm::make_mat4(&pNode->mTransformation.a1)));

		const aiNodeAnim* pNodeAnim = FindNodeAnim(pAnimation, NodeName);

		if (pNodeAnim) {
			// Interpolate scaling and generate scaling transformation matrix
			aiVector3D scaling;
			CalcInterpolatedScaling(scaling, AnimationTime, pNodeAnim);
			glm::mat4 scalingM = glm::mat4(1.0f);
			scalingM = glm::scale(scalingM, glm::vec3(scaling.x, scaling.y, scaling.z));

			// Interpolate rotation and generate rotation transformation matrix
			aiQuaternion rotationQ;
			CalcInterpolatedRotation(rotationQ, AnimationTime, pNodeAnim);
			aiMatrix3x3 a = rotationQ.GetMatrix();
			glm::mat4 rotationM = glm::transpose(glm::make_mat3(&a.a1));

			// Interpolate translation and generate translation transformation matrix
			aiVector3D translation;
			CalcInterpolatedPosition(translation, AnimationTime, pNodeAnim);
			glm::mat4 translationM = glm::mat4(1.0f);
			translationM = glm::translate(translationM, glm::vec3(translation.x, translation.y, translation.z));

			// Combine the above transformations
			NodeTransformation = translationM * rotationM * scalingM;
		}

		glm::mat4 GlobalTransformation = ParentTransform * NodeTransformation;

		if (m_BoneMapping.find(NodeName) != m_BoneMapping.end()) {
			uint BoneIndex = m_BoneMapping[NodeName];
			m_BoneInfo[BoneIndex].FinalTransformation = m_GlobalInverseTransform * GlobalTransformation * m_BoneInfo[BoneIndex].BoneOffset;
		}

		for (uint i = 0; i < pNode->mNumChildren; i++) {
			ReadNodeHeirarchy(AnimationTime, pNode->mChildren[i], GlobalTransformation);
		}
	}


	void Mesh::BoneTransform(float TimeInSeconds, vector<glm::mat4>& Transforms)
	{
		glm::mat4 Identity = glm::mat4(1.0f);
		//Identity.InitIdentity();

		float TicksPerSecond = (float)(m_pScene->mAnimations[0]->mTicksPerSecond != 0 ? m_pScene->mAnimations[0]->mTicksPerSecond : 25.0f);
		float TimeInTicks = TimeInSeconds * TicksPerSecond;
		float AnimationTime = fmod(TimeInTicks, (float)m_pScene->mAnimations[0]->mDuration);

		ReadNodeHeirarchy(AnimationTime, m_pScene->mRootNode, Identity);

		Transforms.resize(m_NumBones);

		for (uint i = 0; i < m_NumBones; i++) {
			Transforms[i] = m_BoneInfo[i].FinalTransformation;
		}
	}


	const aiNodeAnim* Mesh::FindNodeAnim(const aiAnimation* pAnimation, const string NodeName)
	{
		for (uint i = 0; i < pAnimation->mNumChannels; i++) {
			const aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];

			if (string(pNodeAnim->mNodeName.data) == NodeName) {
				return pNodeAnim;
			}
		}

		return NULL;
	}
}
