#pragma once

#include <glm/glm.hpp>

struct Planet {
    glm::vec2 pos;
    glm::vec2 vel;
    float mass;
    float radius;
    float r, g, b;
};