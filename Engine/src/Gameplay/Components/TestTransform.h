#pragma once

#include <glm/glm.hpp>

namespace sixengine {

    struct TestTransform
    {
        glm::mat4 transform;

        TestTransform() : transform(1.0f) {}
        TestTransform(glm::mat4 mat) : transform(mat) {}
    };

}

