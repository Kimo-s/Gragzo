#include <iostream>
#include "gragzo.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "gui_parameters.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/vec3.hpp>
#include <sstream>


gui_parameters guiParameters;

const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 600;
float targetFPS = 60.0;

glm::vec3 cameraPos   = glm::vec3(3.0f, 0.0f,  5.0f);
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

void guiSetup(GLFWwindow* window, ImGuiIO& io){
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();


    ImGui::Begin("Controls");

    ImGui::SliderFloat("Complaince", &guiParameters.complaince, 0.0f, 0.2f); 


    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
    ImGui::End();

    ImGui::Render();
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

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");



    Shader shader("../shaders/vert3d.vs", "../shaders/frag3d.fs");

    std::vector<Vertex> vertices = {
     Vertex{glm::vec3(0.5f,  0.5f, 0.0f), glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, 0.0, 1.0)},
     Vertex{glm::vec3(0.5f, -0.5f, 0.0f), glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, 0.0, 1.0)},
     Vertex{glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, 0.0, 1.0)},
     Vertex{glm::vec3(-0.5f,  0.5f, 0.0f), glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, 0.0, 1.0)},
     Vertex{glm::vec3(1.0f,  0.5f, 1.0f), glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, 0.0, 1.0)},
    }; 

    std::vector<uint>  indices = {  // note that we start from 0!
        0, 1, 3,  // first triangle
        1, 2, 3    // second triangle
    };


    // Mesh bunny("../bunny.obj");

    SoftBody obj("../sphere.obj");
    // SoftBody obj(vertices, indices);


    glEnable(GL_DEPTH_TEST);

    

    // This is the render loop
    // float curTime = glfwGetTime();
    float lastFrameTime = glfwGetTime();
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        guiSetup(window, io);


        // Clear the screen
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();

        glm::mat4 projMat = glm::perspective(glm::radians(60.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 300.0f);
        shader.setMat4("projection", projMat);

        glm::mat4 viewMat = glm::lookAt(cameraPos, cameraFront, cameraUp);
        shader.setMat4("view", viewMat);

        glm::mat4 modelMat = glm::mat4(1.0f);
        shader.setMat4("model", modelMat);

        float dt = glfwGetTime() - lastFrameTime;


        obj.draw();
        obj.simulateTimeStep(1.0/targetFPS);


        lastFrameTime = glfwGetTime();


        float fps = 1.0/dt;
        std::stringstream ss;
        ss << "Gragzp (fps:" << fps << ")";

        while(fps > targetFPS) {
            float dt2 = glfwGetTime() - lastFrameTime;
            fps = 1.0/dt2;
        }

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSetWindowTitle(window, ss.str().c_str());
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

    Yaw   += yoffset;
    Pitch -= xoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    // if (Pitch > 89.0f)
    //     Pitch = 89.0f;
    // if (Pitch < -89.0f)
    //     Pitch = -89.0f;

    float camposr = glm::length(cameraPos);
    cameraPos.x = sin(glm::radians(Yaw)) * sin(glm::radians(Pitch));
    cameraPos.y = cos(glm::radians(Yaw));
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
