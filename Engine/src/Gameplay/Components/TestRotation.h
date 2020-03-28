#pragma once

#include <glm/glm.hpp>

namespace sixengine {

    struct TestRotation
    {
        glm::vec3 axis;
        float speed;

        TestRotation(glm::vec3 rotationAxis, float rotationSpeed) 
            : axis(rotationAxis), speed(rotationSpeed) {}
    };

}