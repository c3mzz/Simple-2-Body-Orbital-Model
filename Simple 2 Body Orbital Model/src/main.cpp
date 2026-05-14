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

        // --- Moon ---
        float moonPositions[] = {
            -0.2f, -0.2f, // 0
             0.2f, -0.2f, // 1
             0.2f,  0.2f, // 2
            -0.2f,  0.2f  // 3
        };
        unsigned int moonIndices[] = { 0, 1, 2, 2, 3, 0 };

        VertexArray moonVAO;
        VertexBuffer moonVBO(moonPositions, sizeof(moonPositions));
        VertexBufferLayout moonLayout;
        moonLayout.Push<float>(2);
        moonVAO.AddBuffer(moonVBO, moonLayout);
        IndexBuffer moonIBO(moonIndices, 6);

        // --- Earth ---
        int segments = 36;
        float radius = 0.5f;
        std::vector<float> earthVertices;
        std::vector<unsigned int> earthIndices;

        earthVertices.push_back(0.0f);
        earthVertices.push_back(0.0f);
        
        const float PI = 3.14159265359f;
        for (int i = 0; i <= segments; i++) {
            float angle = i * (2.0f * PI / segments);
            earthVertices.push_back(radius * std::cos(angle));
            earthVertices.push_back(radius * std::sin(angle));
        }

        for (int i = 1; i <= segments; i++) {
            earthIndices.push_back(0);
            earthIndices.push_back(i);
            earthIndices.push_back(i + 1);
        }

        VertexArray earthVAO;
        VertexBuffer earthVBO(&earthVertices[0], earthVertices.size() * sizeof(float));
        VertexBufferLayout earthLayout;
        earthLayout.Push<float>(2);
        earthVAO.AddBuffer(earthVBO, earthLayout);
        IndexBuffer earthIBO(&earthIndices[0], earthIndices.size());

        glm::mat4 proj = glm::ortho(-4.0f, 4.0f, -3.0f, 3.0f, -1.0f, 1.0f);
        glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));

        // --- Variable ---
        float moonX = 2.0f;
        float moonY = 0.0f;
        float moonVx = 0.0f;
        float moonVy = 0.02f;

        float earthMass = 10.0f;
        float G = 0.0001f;
        // ----------------

        Renderer renderer;

        while (!glfwWindowShouldClose(window))
        {
            renderer.Clear();

            float dx = 0.0f - moonX;
            float dy = 0.0f - moonY;
            float distance = std::sqrt(dx * dx + dy * dy);

            if (distance > 0.1f)
            {
                float force = (G * earthMass) / (distance * distance);
                float ax = force * (dx / distance);
                float ay = force * (dy / distance);

                moonVx += ax;
                moonVy += ay;
            }

            moonX += moonVx;
            moonY += moonVy;

            glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(moonX, moonY, 0.0f));
            glm::mat4 mvp = proj * view * model;
            
            shader.Bind();

            // --- Earth ---
            glm::mat4 earthModel = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
            glm::mat4 earthMVP = proj * view * earthModel;
            
            shader.SetUniformMat4("u_MVP", earthMVP);
            shader.SetUniform4f("u_Color", 0.2f, 0.3f, 0.8f, 1.0f);

            renderer.Draw(earthVAO, earthIBO, shader);


            // --- Moon ---
            glm::mat4 moonModel = glm::translate(glm::mat4(1.0f), glm::vec3(moonX, moonY, 0.0f));
            glm::mat4 moonMVP = proj * view * moonModel;
            
            shader.SetUniformMat4("u_MVP", moonMVP);
            shader.SetUniform4f("u_Color", 0.7f, 0.7f, 0.7f, 1.0f);

            renderer.Draw(moonVAO, moonIBO, shader);


            glfwSwapBuffers(window);
            glfwPollEvents();
        }

    }
    glfwTerminate();
    return 0;
}
