#include "pch.h"
#include "Model.h"
#include <glad/glad.h> 
#include "stb_image.h"

namespace sixengine {

	Model::Model()
		: m_MinAxis(10000.0f), m_MaxAxis(0.0001f)
	{
		VAO = nullptr;
		m_NumBones = 0;
		m_Scene = NULL;
	}

	Model::Model(const std::string& filename, unsigned ID)
		: m_MinAxis(10000.0f), m_MaxAxis(0.0001f)
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

	bool Model::LoadModel(const std::string& filename)
	{
		// Release the previously loaded mesh (if it exists)
		Clear();
		
		bool ret = false;

		m_Scene = m_Importer.ReadFile(filename.c_str(), aiProcess_Triangulate |
			aiProcess_GenSmoothNormals |
			aiProcess_FlipUVs |
			aiProcess_JoinIdenticalVertices | 
			aiProcess_CalcTangentSpace);
		if (m_Scene) {
			m_GlobalInverseTransform = glm::transpose(glm::make_mat4(&m_Scene->mRootNode->mTransformation.a1));
			m_GlobalInverseTransform = glm::inverse(m_GlobalInverseTransform);
			
			m_Directory = filename.substr(0, filename.find_last_of('/'));
			m_Name = filename.substr(filename.find_last_of('/'));
			ret = InitFromScene(m_Scene, filename);
		}
		else {
			LOG_ERROR("Error parsing {0}: {1}", filename.c_str(), m_Importer.GetErrorString());
		}

		return ret;
	}

	bool Model::InitFromScene(const aiScene* scene, const std::string& filename)
	{
		m_Entries.resize(scene->mNumMeshes);

		uint numVertices = 0;
		m_TotalNumIndices = 0;

		uint verticesPerPrim = 3;

		// Count the number of vertices and indices
		for (uint i = 0; i < m_Entries.size(); i++) {
			//m_Entries[i].MaterialIndex = scene->mMeshes[i]->mMaterialIndex;
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
			InitMesh(i, mesh/*, vertices, indices*/);
		}
		
		LoadGlobalPositions(m_Scene->mRootNode, glm::mat4(1.0f));

		return true;
	}

	void Model::InitMesh(uint meshIndex, const aiMesh* mesh/*, std::vector<Vertex>& vertices, std::vector<uint>& indices*/)
	{
		// Populate the vertex attribute vectors
		for (uint i = 0; i < mesh->mNumVertices; i++) {

			Vertex vertex;
			glm::vec3 vector;

			// Postion
			vector = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
			vertex.Position = vector;

			// Bounding box
			m_MinAxis.x = min(m_MinAxis.x, vector.x);
			m_MinAxis.y = min(m_MinAxis.y, vector.y);
			m_MinAxis.z = min(m_MinAxis.z, vector.z);

			m_MaxAxis.x = max(m_MaxAxis.x, vector.x);
			m_MaxAxis.y = max(m_MaxAxis.y, vector.y);
			m_MaxAxis.z = max(m_MaxAxis.z, vector.z);

			// Normals
			if (mesh->mNormals)
				vector = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };
			else
				vector = glm::vec3(0.0f);
			vertex.Normal = vector;

			if (mesh->mTangents)
				vector = { mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z };
			else
				vector = glm::vec3(0.0f);
			vertex.Tangent = vector;

			if (mesh->mBitangents)
				vector = { mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z };
			else
				vector = glm::vec3(0.0f);
			vertex.Bitangent = vector;

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
				m_BoneInfo[boneIndex].Name = mesh->mBones[i]->mName.C_Str();
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

	uint Model::FindPosition(float animationTime, const aiNodeAnim* nodeAnim)
	{
		for (uint i = 0; i < nodeAnim->mNumPositionKeys - 1; i++) {
			if (animationTime < (float)nodeAnim->mPositionKeys[i + 1].mTime) {
				return i;
			}
		}

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
		//assert(nodeAnim->mNumScalingKeys > 0);

		for (uint i = 0; i < nodeAnim->mNumScalingKeys - 1; i++) {
			if (animationTime < (float)nodeAnim->mScalingKeys[i + 1].mTime) {
				return i;
			}
		}

		//assert(0);

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
		//assert(nextpositionIndex < nodeAnim->mNumPositionKeys);
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
		//assert(nextRotationIndex < nodeAnim->mNumRotationKeys);
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
		//assert(nextScalingIndex < nodeAnim->mNumScalingKeys);
		float deltaTime = (float)(nodeAnim->mScalingKeys[nextScalingIndex].mTime - nodeAnim->mScalingKeys[scalingIndex].mTime);
		float factor = (animationTime - (float)nodeAnim->mScalingKeys[scalingIndex].mTime) / deltaTime;
		//assert(Factor >= 0.0f && Factor <= 1.0f);
		const aiVector3D& start = nodeAnim->mScalingKeys[scalingIndex].mValue;
		const aiVector3D& end = nodeAnim->mScalingKeys[nextScalingIndex].mValue;
		aiVector3D delta = end - start;
		out = start + factor * delta;
	}

	void Model::ReadNodeHierarchy(float animationTime, const aiNode* node, const glm::mat4& parentTransform, AnimationEntry* animation, std::vector<glm::mat4>& transforms)
	{
		std::string nodeName(node->mName.data);

		//const aiAnimation* animation = m_AnimationsMapping[animationName]->animation;

		glm::mat4 nodeTransformation(glm::transpose(glm::make_mat4(&node->mTransformation.a1)));


		const aiNodeAnim* nodeAnim = nullptr;		

		if (animation->nodeAnimationMapping.find(nodeName) != animation->nodeAnimationMapping.end())
			nodeAnim = animation->nodeAnimationMapping[nodeName];

		if (nodeAnim) 
		{
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

		if (m_BoneMapping.find(nodeName) != m_BoneMapping.end()) 
		{
			uint boneIndex = m_BoneMapping[nodeName];

			if (m_BoneInfo[boneIndex].FreeBone)
			{
				transforms[boneIndex] = m_GlobalInverseTransform * m_BoneInfo[boneIndex].GlobalTransformation * m_BoneInfo[boneIndex].BoneOffset;
				globalTransformation = m_BoneInfo[boneIndex].GlobalTransformation;
				m_BoneInfo[boneIndex].FinalTransformation = transforms[boneIndex];
			}
			else
			{
				transforms[boneIndex] = m_GlobalInverseTransform * globalTransformation * m_BoneInfo[boneIndex].BoneOffset;
				m_BoneInfo[boneIndex].FinalTransformation = transforms[boneIndex];
			}
		}

		for (uint i = 0; i < node->mNumChildren; i++) {
			ReadNodeHierarchy(animationTime, node->mChildren[i], globalTransformation, animation, transforms);
		}
	}	

	void Model::LoadGlobalPositions(const aiNode* node, const glm::mat4& parentTransform)
	{
		std::string nodeName(node->mName.data);

		glm::mat4 nodeTransformation(glm::transpose(glm::make_mat4(&node->mTransformation.a1)));
		glm::mat4 globalTransformation = parentTransform * nodeTransformation;

		if (m_BoneMapping.find(nodeName) != m_BoneMapping.end())
		{
			uint boneIndex = m_BoneMapping[nodeName];
			m_BoneInfo[boneIndex].GlobalTransformation = globalTransformation;
		}

		for (uint i = 0; i < node->mNumChildren; i++) 
			LoadGlobalPositions(node->mChildren[i], globalTransformation);
	}


	void Model::BoneTransform(AnimationEntry* currentAnimation, AnimationEntry* previousAnimation, std::vector<glm::mat4>& transforms)
	{	
		float currentTimeInTicks = currentAnimation->timer * currentAnimation->ticksPerSecond;
		float previousTimeInTicks = previousAnimation->timer * previousAnimation->ticksPerSecond;

		float currentAnimationTime = fmod(currentTimeInTicks, (float)currentAnimation->animation->mDuration);
		float previousAnimationTime = fmod(previousTimeInTicks, (float)previousAnimation->animation->mDuration);

		if (currentAnimation && previousAnimation)
		{
			std::vector<glm::mat4> blendLayer1(m_NumBones);
			std::vector<glm::mat4> blendLayer2(m_NumBones);
			float blendProgress = 0.0f;

			transforms.resize(m_NumBones);
			ReadNodeHierarchy(currentAnimationTime, currentAnimation->scene->mRootNode, glm::mat4(1.0f), currentAnimation, transforms);
			for (uint i = 0; i < m_NumBones; i++)
				blendLayer1[i] = transforms[i];

			if (currentAnimation->blendLength > 0.0f)
			{
				transforms.clear();
				transforms.resize(m_NumBones);

				ReadNodeHierarchy(previousAnimationTime, previousAnimation->scene->mRootNode, glm::mat4(1.0f), previousAnimation, transforms);
				for (uint i = 0; i < m_NumBones; i++)
					blendLayer2[i] = transforms[i];

				blendProgress = ((currentAnimation->timer / currentAnimation->blendLength) > 1.0f) ? 1.0f : (currentAnimation->timer / 1.0f);
			}
			else
				blendProgress = 1.0f;

			for (uint i = 0; i < m_NumBones; i++)
				transforms[i] = blendLayer1[i] *blendProgress + blendLayer2[i] * (1.0f - blendProgress);
		}
		else
		{
			transforms.clear();
			transforms.resize(m_NumBones);

			if (!currentAnimation)
				ReadNodeHierarchy(currentAnimationTime, m_Scene->mRootNode, glm::mat4(1.0f), currentAnimation, transforms);
			else
				ReadNodeHierarchy(currentAnimationTime, currentAnimation->scene->mRootNode, glm::mat4(1.0f), currentAnimation, transforms);

		}
	}

	void Model::SetFrustumInfinity()
	{
		float inf = std::numeric_limits<float>::infinity();
		m_MinAxis = glm::vec3(-inf, -inf, -inf);
		m_MaxAxis = glm::vec3(inf, inf, inf);
	}
	
}
