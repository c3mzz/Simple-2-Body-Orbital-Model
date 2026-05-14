#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "Renderer.h"
#include "Planet.h"
#include "PhysicsEngine.h"

int main()
{
    GLFWwindow* window;
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    glfwSwapInterval(1);

    if (glewInit() != GLEW_OK)
        std::cout << "Error!" << std::endl;

    std::cout << glGetString(GL_VERSION) << std::endl;
    {
        Shader shader("res/shaders/Basic.shader");
        shader.Bind();


        float camX = 0.0f;
        float camY = 0.0f;
        float camZoom = 1.0f;
        float camSpeed = 0.05f;


        // --- Gen Circle ---
        int segments = 36;
        float radius = 1.0f;
        std::vector<float> circleVertices;
        std::vector<unsigned int> circleIndices;

        circleVertices.push_back(0.0f);
        circleVertices.push_back(0.0f);

        const float PI = 3.14159265359f;
        for (int i = 0; i <= segments; i++) {
            float angle = i * (2.0f * PI / segments);
            circleVertices.push_back(radius * std::cos(angle));
            circleVertices.push_back(radius * std::sin(angle));
        }

        for (int i = 1; i <= segments; i++) {
            circleIndices.push_back(0);
            circleIndices.push_back(i);
            circleIndices.push_back(i + 1);
        }

        VertexArray masterVAO;
        VertexBuffer masterVBO(&circleVertices[0], circleVertices.size() * sizeof(float));
        VertexBufferLayout circleLayout;
        circleLayout.Push<float>(2);
        masterVAO.AddBuffer(masterVBO, circleLayout);
        IndexBuffer masterIBO(&circleIndices[0], circleIndices.size());
        // ------------------


        glm::mat4 proj = glm::ortho(-4.0f, 4.0f, -3.0f, 3.0f, -1.0f, 1.0f);
        glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));

        std::vector<Planet> solarSystem;

        //                               pos                     vel                mass     radius    r     g     b
        solarSystem.push_back({ glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, -0.01f),  1000.0f,  0.5f,    0.2f, 0.3f, 0.8f }); // Earth
        solarSystem.push_back({ glm::vec2(2.0f, 0.0f), glm::vec2(0.0f, 0.22f),     10.0f,  0.15f,   0.7f, 0.7f, 0.7f }); // Moon 1
        //solarSystem.push_back({ glm::vec2(-2.5f, 1.0f), glm::vec2(0.0f, -0.15f),    50.0f,  0.25f,   0.8f, 0.2f, 0.2f }); // Moon 2

        PhysicsEngine physics;
        Renderer renderer;

        while (!glfwWindowShouldClose(window))
        {
            renderer.Clear();

            // --- Camera Input ---
            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camY += camSpeed / camZoom;
            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camY -= camSpeed / camZoom;
            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) camX -= camSpeed / camZoom;
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) camX += camSpeed / camZoom;

            // --- Zooming ---
            if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) camZoom *= 1.01f;
            if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) camZoom *= 0.99f;

            glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(-camX, -camY, 0.0f));

            float aspect = 4.0f / 3.0f;
            float width = 4.0f / camZoom;
            float height = 3.0f / camZoom;
            glm::mat4 proj = glm::ortho(-width, width, -height, height, -1.0f, 1.0f);

            physics.UpdateRK4(solarSystem, 0.5f);

            for (auto& p : solarSystem) {
                p.trail.push_front(p.pos);
                if (p.trail.size() > p.maxTrailSize) {
                    p.trail.pop_back();
                }
            }

            shader.Bind();
            for (const Planet& p : solarSystem)
            {
                for (size_t i = 0; i < p.trail.size(); ++i) {
                    float sizeFactor = 1.0f - ((float)i / p.trail.size());
                    float trailScale = p.radius * 0.4f * sizeFactor;

                    glm::mat4 model = glm::mat4(1.0f);
                    model = glm::translate(model, glm::vec3(p.trail[i].x, p.trail[i].y, 0.0f));
                    model = glm::scale(model, glm::vec3(trailScale, trailScale, 1.0f));

                    shader.SetUniformMat4("u_MVP", proj * view * model);
                    shader.SetUniform4f("u_Color", p.r * 0.6f, p.g * 0.6f, p.b * 0.6f, 1.0f);

                    glm::mat4 mvp = proj * view * model;
                    shader.SetUniformMat4("u_MVP", mvp);
                    renderer.Draw(masterVAO, masterIBO, shader);
                }

                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(p.pos.x, p.pos.y, 0.0f));
                model = glm::scale(model, glm::vec3(p.radius, p.radius, 1.0f));
                glm::mat4 mvp = proj * view * model;

                shader.SetUniformMat4("u_MVP", mvp);
                shader.SetUniform4f("u_Color", p.r, p.g, p.b, 1.0f);

                renderer.Draw(masterVAO, masterIBO, shader);
            }

            glfwSwapBuffers(window);
            glfwPollEvents();
        }

    }
    glfwTerminate();
    return 0;
}
