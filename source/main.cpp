#include <iostream>
#include "gragzo.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/vec3.hpp>

const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 600;

glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f,  3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);

float lastX = WIDTH / 2.0f;
float lastY = HEIGHT / 2.0f;
bool firstMouse = true;

bool leftMouseHeld = false;

float Yaw   = -90.0f;
float Pitch = 0.0f;

void mouse_callback(GLFWwindow* window, double xpos, double ypos);

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);



    // Construct the window
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Gragzo", nullptr, nullptr);
    if (!window)
    {
        std::cout << "Failed to create the GLFW window\n";
        glfwTerminate();
    }

    glfwMakeContextCurrent(window);
    glfwSetCursorPosCallback(window, mouse_callback);

    glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD\n";
        return -1;
    }

    // Handle view port dimensions
    glViewport(0, 0, WIDTH, HEIGHT);
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height)
    {
        glViewport(0, 0, width, height);
    });

    Shader shader("../shaders/vert3d.vs", "../shaders/frag3d.fs");


    Vertex data[] = {
        Vertex{glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3(1.0, 0.0, 0.0)},
        Vertex{glm::vec3(0.5f, -0.8f, 0.0f), glm::vec3(0.0, 1.0, 0.0)},
        Vertex{glm::vec3(0.0f,  0.9f, 0.0f), glm::vec3(0.0, 0.0, 1.0)}
    };


    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // // position attribute
    glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)offsetof(Vertex, col));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    
    // This is the render loop
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        // Clear the screen
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shader.use();

        glm::mat4 projMat = glm::perspective(glm::radians(60.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
        shader.setMat4("projection", projMat);

        glm::mat4 viewMat = glm::lookAt(cameraPos, cameraFront, cameraUp);
        shader.setMat4("view", viewMat);

        glm::mat4 modelMat = glm::mat4(1.0f);
        shader.setMat4("model", modelMat);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    shader.terminate();

    glfwTerminate();
    return 0;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE){
        return;
    }

    xpos = static_cast<float>(xpos);
    ypos = static_cast<float>(ypos);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    xoffset *= 0.2;
    yoffset *= 0.2;

    Yaw   -= xoffset;
    Pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (Pitch > 89.0f)
        Pitch = 89.0f;
    if (Pitch < -89.0f)
        Pitch = -89.0f;

    float camposr = cameraPos.length();
    cameraPos.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    cameraPos.y = sin(glm::radians(Pitch));
    cameraPos.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    cameraPos = camposr * glm::normalize(cameraPos);
    // cameraFront = glm::normalize(cameraFront);
    cameraFront = -glm::normalize(cameraPos);

    glm::vec3 right = glm::normalize(glm::cross(cameraFront, cameraUp));
    cameraUp = glm::normalize(glm::cross(right, cameraFront));
}
