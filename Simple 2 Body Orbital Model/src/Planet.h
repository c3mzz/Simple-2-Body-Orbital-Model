#pragma once

#include <glm/glm.hpp>
#include <deque>
#include <vector>

struct Planet {
    glm::vec2 pos;
    glm::vec2 vel;
    float mass;
    float radius;
    float r, g, b;

    std::deque<glm::vec2> trail;
    size_t maxTrailSize = 100;
};