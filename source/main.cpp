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
void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
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
    glfwSetScrollCallback(window, mouse_scroll_callback);

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


    Mesh bunny("../bunny.obj");

    SoftBody obj("../bunny.obj");

    glEnable(GL_DEPTH_TEST);

    

    // This is the render loop
    // float curTime = glfwGetTime();
    float lastFrameTime = glfwGetTime();
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        // Clear the screen
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();

        glm::mat4 projMat = glm::perspective(glm::radians(60.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
        shader.setMat4("projection", projMat);

        glm::mat4 viewMat = glm::lookAt(cameraPos, cameraFront, cameraUp);
        shader.setMat4("view", viewMat);

        glm::mat4 modelMat = glm::mat4(1.0f);
        shader.setMat4("model", modelMat);

        float dt = glfwGetTime() - lastFrameTime;

        obj.draw();
        // bunny.draw();
        obj.simulateTimeStep(dt);

        lastFrameTime = glfwGetTime();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    shader.terminate();

    glfwTerminate();
    return 0;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE){
        leftMouseHeld = false;
        return;
    }

    if(leftMouseHeld == false) {
        leftMouseHeld = true;
        lastX = xpos;
        lastY = ypos;
    }

    xpos = static_cast<float>(xpos);
    ypos = static_cast<float>(ypos);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = lastX - xpos;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    xoffset *= 0.2;
    yoffset *= 0.2;

    Yaw   += xoffset;
    Pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (Pitch > 89.0f)
        Pitch = 89.0f;
    if (Pitch < -89.0f)
        Pitch = -89.0f;

    float camposr = glm::length(cameraPos);
    cameraPos.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    cameraPos.y = sin(glm::radians(Pitch));
    cameraPos.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    cameraPos = camposr * glm::normalize(cameraPos);
    cameraFront = -glm::normalize(cameraPos);

    glm::vec3 right = glm::normalize(glm::cross(cameraFront, cameraUp));
    cameraUp = glm::normalize(glm::cross(right, cameraFront));
}



void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    cameraPos = (glm::length(cameraPos) + static_cast<float>(yoffset) * 0.1f) * glm::normalize(cameraPos);
}
