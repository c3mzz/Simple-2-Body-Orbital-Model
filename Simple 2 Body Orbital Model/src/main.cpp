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

            physics.UpdateRK4(solarSystem, 0.1f);

            shader.Bind();
            for (const Planet& p : solarSystem)
            {
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
