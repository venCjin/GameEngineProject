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
			LOG_INFO("___curr anim: {0}", m_CurrentAnimationName);
			if (m_AnimationsMapping.find(m_CurrentAnimationName) != m_AnimationsMapping.end())
				return m_AnimationsMapping[m_CurrentAnimationName];

			return nullptr;
		}
		
		AnimationEntry* GetPreviousAnimation()
		{
			LOG_INFO("___prev anim: {0}", m_PreviousAnimationName);

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
			LOG_INFO("___new anim name: {0}", m_CurrentAnimationName);
			//previousAnimationTimer = currentAnimationTimer;
			m_AnimationsMapping[m_CurrentAnimationName]->timer = 0.0f;
		}

		// TODO: czy je�li przypisz� na starcie do singleCycle false, to b�dzie si� to zmienia�o przy wywo�aniu z true?
		bool LoadAnimation(const std::string & filename, std::string name, bool singleCycle)
		{
			AnimationEntry* ae = new AnimationEntry(filename, name);

			if (m_AnimationsMapping.find(name) == m_AnimationsMapping.end())
			{
				m_AnimationsMapping[name] = ae;
				m_AnimationsMapping[name]->singleCycle = singleCycle;
			}
			else
				std::cout << "Already exists" << std::endl;

			return false;
		}


	};

}