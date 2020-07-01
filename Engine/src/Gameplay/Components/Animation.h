#pragma once
#include <string>

namespace sixengine {

	struct AnimationEntry
	{
		AnimationEntry()
		{
			scene = nullptr;
			animation = nullptr;
		}

		AnimationEntry(const std::string& filename, std::string givenName)
		{
			name = givenName;
			scene = importer.ReadFile(filename.c_str(), aiProcess_FlipUVs);
			if (scene->HasAnimations())
			{
				animation = scene->mAnimations[0];
				LoadAnimationNodes();
				ticksPerSecond = (float)(animation->mTicksPerSecond != 0 ? animation->mTicksPerSecond : 25.0f);
				duration = animation->mDuration;
			}


			if (!scene->HasAnimations() || animation == nullptr || nodeAnimationMapping.size() == 0)
				LOG_ERROR("AnimationEntry has not animation / is nullptr / has no nodes: {0}", name);

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
		float timer = 0.0f;
		bool singleCycle = false;
		float ticksPerSecond = 0.0f;
		float duration = 0.0f;
		std::string name;
		float blendLength = 0.0f;
		float timeScale = 1.0f;
	};


	class Animation
	{
	private:
	public:			

		std::string m_CurrentAnimationName = "idle";
		std::string m_PreviousAnimationName = "idle";

		std::map<std::string, AnimationEntry*> m_AnimationsMapping;

		AnimationEntry* GetCurrentAnimation()
		{
			if (m_AnimationsMapping.find(m_CurrentAnimationName) != m_AnimationsMapping.end())
				return m_AnimationsMapping[m_CurrentAnimationName];

			return nullptr;
		}
		
		AnimationEntry* GetPreviousAnimation()
		{
			if (m_AnimationsMapping.find(m_PreviousAnimationName) != m_AnimationsMapping.end())
				return m_AnimationsMapping[m_PreviousAnimationName];

			return nullptr;
		}

		void ChangeAnimation(std::string newAnimationName)
		{
			if (m_CurrentAnimationName == newAnimationName)
				return;

			m_PreviousAnimationName = m_CurrentAnimationName;
			m_CurrentAnimationName = newAnimationName;
			m_AnimationsMapping[m_CurrentAnimationName]->timer = 0.0f;
		}

		bool LoadAnimation(const std::string & filename, std::string name, bool singleCycle = false, float blendLength = 0.5f, float timeScale = 1.0f)
		{
			AnimationEntry* ae = new AnimationEntry(filename, name);

			if (m_AnimationsMapping.find(name) == m_AnimationsMapping.end())
			{
				m_AnimationsMapping[name] = ae;
				m_AnimationsMapping[name]->singleCycle = singleCycle;
				m_AnimationsMapping[name]->blendLength = blendLength;
				m_AnimationsMapping[name]->timeScale = timeScale;
			}
			else
				std::cout << "Already exists" << std::endl;

			return false;
		}


	};

}