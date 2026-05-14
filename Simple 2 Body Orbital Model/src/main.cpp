#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

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

        float positions[] = {
            -0.5f, -0.5f, // 0
             0.5f, -0.5f, // 1
             0.5f,  0.5f, // 2
            -0.5f,  0.5f  // 3
        };
        unsigned int indices[] = { 0, 1, 2, 2, 3, 0 };


        VertexArray earthVAO;
        VertexBuffer earthVBO(positions, sizeof(positions));
        VertexBufferLayout layout;
        layout.Push<float>(2);
        earthVAO.AddBuffer(earthVBO, layout);
        IndexBuffer earthIBO(indices, 6);

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
            shader.SetUniformMat4("u_MVP", mvp);

            renderer.Draw(earthVAO, earthIBO, shader);

            glfwSwapBuffers(window);
            glfwPollEvents();
        }

    }
    glfwTerminate();
    return 0;
}
