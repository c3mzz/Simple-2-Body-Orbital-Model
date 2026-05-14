#include "PhysicsEngine.h"
#include <cmath>

std::vector<glm::vec2> PhysicsEngine::GetAccelerations(const std::vector<glm::vec2>& positions, const std::vector<Planet>& planets)
{
    int n = planets.size();
    std::vector<glm::vec2> acc(n, glm::vec2(0.0f));

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (i == j) continue;

            glm::vec2 dir = positions[j] - positions[i];
            float distSq = glm::dot(dir, dir);
            float dist = std::sqrt(distSq);

            if (dist > 0.05f) {
                float force = (G * planets[j].mass) / distSq;
                acc[i] += force * (dir / dist);
            }
        }
    }
    return acc;
}

void PhysicsEngine::UpdateRK4(std::vector<Planet>& planets, float dt)
{
    int n = planets.size();

    std::vector<glm::vec2> pos_k(n), vel_k(n);

    // --- K1 ---
    for (int i = 0; i < n; ++i) {
        pos_k[i] = planets[i].pos;
        vel_k[i] = planets[i].vel;
    }
    std::vector<glm::vec2> acc_k1 = GetAccelerations(pos_k, planets);

    // --- K2 ---
    for (int i = 0; i < n; ++i) {
        pos_k[i] = planets[i].pos + vel_k[i] * (dt * 0.5f);
        vel_k[i] = planets[i].vel + acc_k1[i] * (dt * 0.5f);
    }
    std::vector<glm::vec2> acc_k2 = GetAccelerations(pos_k, planets);

    // --- K3 ---
    for (int i = 0; i < n; ++i) {
        pos_k[i] = planets[i].pos + vel_k[i] * (dt * 0.5f);
        vel_k[i] = planets[i].vel + acc_k2[i] * (dt * 0.5f);
    }
    std::vector<glm::vec2> acc_k3 = GetAccelerations(pos_k, planets);

    // --- K4 ---
    for (int i = 0; i < n; ++i) {
        pos_k[i] = planets[i].pos + vel_k[i] * dt;
        vel_k[i] = planets[i].vel + acc_k3[i] * dt;
    }
    std::vector<glm::vec2> acc_k4 = GetAccelerations(pos_k, planets);

    // ----------

    for (int i = 0; i < n; ++i) {
        glm::vec2 vk1 = planets[i].vel;
        glm::vec2 vk2 = planets[i].vel + acc_k1[i] * (dt * 0.5f);
        glm::vec2 vk3 = planets[i].vel + acc_k2[i] * (dt * 0.5f);
        glm::vec2 vk4 = planets[i].vel + acc_k3[i] * dt;

        planets[i].pos += (dt / 6.0f) * (vk1 + 2.0f * vk2 + 2.0f * vk3 + vk4);
        planets[i].vel += (dt / 6.0f) * (acc_k1[i] + 2.0f * acc_k2[i] + 2.0f * acc_k3[i] + acc_k4[i]);
    }
}