#pragma once

#include <vector>
#include "Planet.h"

class PhysicsEngine {
public:
    float G = 0.0001f;
    void UpdateRK4(std::vector<Planet>& planets, float dt);

private:
    std::vector<glm::vec2> GetAccelerations(const std::vector<glm::vec2>& positions, const std::vector<Planet>& planets);
};