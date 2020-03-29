#pragma once

#include <glm/glm.hpp>
#include "Gameplay\GraphNode.h"

namespace sixengine {

    class Transform
    {
    private:
        GraphNode* m_Parent;
        glm::mat4 m_Local;
        glm::mat4 m_World;

    public:
        Transform(GraphNode* parent) : m_Parent(parent), m_Local(glm::mat4(1.0f)), m_World(glm::mat4(1.0f)) {}
        Transform(GraphNode* parent, glm::mat4 local, glm::mat4 world) : m_Parent(parent), m_Local(local), m_World(world) {}

        glm::mat4 Combine(Transform& other)
        {
            return other.m_World * m_World * m_Local;
        }

        glm::mat4 Combine()
        {
            return m_World * m_Local;
        }

        void SetLocal(glm::mat4 local)
        {
            m_Local = local;
        }

        void SetWorld(glm::mat4 world)
        {
            m_World = world;
            m_Parent->SetDirty(true);
        }

        const glm::mat4& GetLocal()
        {
            return m_Local;
        }

        const glm::mat4& GetWorld()
        {
            return m_World;
        }

    };

}

