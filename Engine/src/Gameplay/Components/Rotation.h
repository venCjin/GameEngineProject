#pragma once

#include <glm/glm.hpp>

namespace sixengine {

    struct Rotation
    {
        glm::vec3 axis;
        float speed;

        Rotation(glm::vec3 rotationAxis, float rotationSpeed) 
            : axis(rotationAxis), speed(rotationSpeed) {}
    };

}