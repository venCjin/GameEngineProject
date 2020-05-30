#pragma once

#include <glm/glm.hpp>
#include "Core/ISerializable.h"

namespace sixengine {

    struct Rotation : public ISerializable
    {
        glm::vec3 axis;
        float speed;

        Rotation(glm::vec3 rotationAxis, float rotationSpeed) 
            : axis(rotationAxis), speed(rotationSpeed) {}

        Rotation(GameObject* go) {}

        virtual void Load(std::iostream& stream)
        {
            float x, y, z, s;
            stream >> x >> y >> z >> s;
            axis = { x, y, z };
            speed = s;
        }
        virtual void Save(std::iostream& stream)
        {
            //TODO:
        }
    };

}