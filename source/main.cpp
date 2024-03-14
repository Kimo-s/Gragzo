#include <iostream>
#include "gragzo.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "gui_parameters.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <Texture.hpp>
#include <sstream>

using namespace gra;
gui_parameters guiParameters;

unsigned int WIDTH = 800;
unsigned int HEIGHT = 600;
float targetFPS = 1/60.0;

glm::vec3 Position = glm::vec3(0.0f, 0.0f, 10.0f);
glm::vec3 Orientation = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);

float speed = 0.1f;
float sensitivity = 8.0f;

Scene scene;
gra::CollisionDetector detector;
Shader* shader;


bool firstClick = true;

Camera mainCamera(glm::vec3(0.0f, 0.0f, 10.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), WIDTH, HEIGHT, 300.0f, 0.1f, 60.0f);

void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void processInput(GLFWwindow *window);
void window_size_callback(GLFWwindow* window, int width, int height);
void processCamera(GLFWwindow *window, Camera* camera);

void setColor(std::shared_ptr<Mesh> mesh, v3 col){
    mesh->specularTexture = new Texture<unsigned char>(glm::vec4(col.x, col.y, col.z, 1.0));
    mesh->updateBuffers();
}

void guiSetup(GLFWwindow* window, ImGuiIO& io){
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();


    ImGui::Begin("Controls");

    ImGui::SliderFloat("Complaince", &guiParameters.complaince, 0.0f, 1.0f); 
    ImGui::SliderFloat("Restitution", &guiParameters.restitution, 0.0f, 1.0f); 

    if(ImGui::Button("Freeze All Bodies")){
        for(int i = 0; i < scene.meshes.size(); i++){
            scene.meshes[i]->v = v3(0.0);
        }
    }

    if(ImGui::CollapsingHeader("Camera Settings")){
        ImGui::SliderFloat("Sensitivity", &mainCamera.rotationSensitivity, 10.0f, 200.0f);
    }


    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
    ImGui::End();

    ImGui::Render();
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
    glfwSetScrollCallback(window, mouse_scroll_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetWindowSizeCallback(window, window_size_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);;

    // glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE);

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
        HEIGHT = height;
        WIDTH = width;
    });

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");


    shader = new Shader("../shaders/vert3d.vs", "../shaders/frag3d.fs");
    

    // std::vector<Vertex> vertices = {
    //  Vertex{glm::vec3(0.5f,  0.5f, 0.0f), glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, 0.0, 1.0)},
    //  Vertex{glm::vec3(0.5f, -0.5f, 0.0f), glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, 0.0, 1.0)},
    //  Vertex{glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, 0.0, 1.0)},
    //  Vertex{glm::vec3(-0.5f,  0.5f, 0.0f), glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, 0.0, 1.0)},
    //  Vertex{glm::vec3(1.0f,  0.5f, 1.0f), glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, 0.0, 1.0)},
    // }; 

    // std::vector<uint>  indices = {  // note that we start from 0!
    //     0, 1, 3,  // first triangle
    //     1, 2, 3    // second triangle
    // };

    std::shared_ptr<RigidBody> obj = std::make_shared<RigidBody>("../bunny.obj", *shader);
    // std::shared_ptr<RigidBody> obj2 = std::make_shared<RigidBody>("../cubemesh.obj", *shader);
    obj->x = glm::vec3(0.0,-3.0,0.0);
    obj->setColor(glm::vec4(0.1, 0.3, 0.6, 1.0));
    // obj2->x = glm::vec3(0.0,2.0,0.0);
    // obj->isStatic = true;
    // setColor(obj2, v3(0.0, 1.0, 0.5)); 
    // obj2->x = glm::vec3(0.0,0.0,0.0);
    // obj2->isStatic = true;


    scene.addMesh(obj);
    // scene.addMesh(obj2);

    detector.addScene(scene);



    // SoftBody softobj("../sphere.obj", *shader);

    glEnable(GL_DEPTH_TEST);


    // This is the render loop
    float lastFrameTime = 0.0f;
    float delta = 0.0f;
    float timeSum = 0.0f;
    while (!glfwWindowShouldClose(window))
    {

        guiSetup(window, io);

        float currentTime = glfwGetTime();
        delta = currentTime- lastFrameTime;
        lastFrameTime = currentTime;
        timeSum += delta;
        if(timeSum >= targetFPS){
            // Clear the screen
            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            mainCamera.proccessWindow(window);
            mainCamera.setShaderValues(shader);
        
            timeSum = 0;
            
            scene.simulatePhysics(targetFPS);
            scene.drawScene();

            // for(int i = 0; i < scene.meshes.size(); i++){
            //     scene.meshes[i]->applyForce(glm::vec3(0.0, -9.8, 0.0));
            // }
            detector.detectCollisions();

            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            glfwSwapBuffers(window);
            glfwPollEvents();
        }

    }

    shader->terminate();

    glfwTerminate();
    return 0;
}



void processCamera(GLFWwindow *window, Camera* camera){


    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{

        double newMouseX, newMouseY;
        glfwGetCursorPos(window, &newMouseX, &newMouseY);
        if(firstClick == false){
            firstClick = true;
            float normalMouseX = (newMouseX - camera->height/2) / camera->height;
            float normalMouseY = (newMouseY - camera->width/2) / camera->width;
            camera->curScreenX = normalMouseX;
            camera->curScreenY = normalMouseY;
        }
        camera->rotAroundCenter(newMouseX, newMouseY);

    } else if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE){
        if(firstClick == true){
            firstClick = false;
        }
    }

    if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS){
        camera->worldPosition.y += 0.01f;
    }

}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{

    // std::cout << xpos << std::endl;
}

void window_size_callback(GLFWwindow* window, int width, int height)
{
    mainCamera.width = width;
    mainCamera.height = height;
}

void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    Position = (glm::length(Position) + static_cast<float>(-yoffset) * 0.4f) * glm::normalize(Position);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods){
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS){

        std::shared_ptr<RigidBody> obj2 = std::make_shared<RigidBody>("../sphere.obj", *shader);
        obj2->x = mainCamera.worldPosition + glm::normalize(mainCamera.viewDirection);
        obj2->v = glm::normalize(mainCamera.viewDirection)*8.0f;
        setColor(obj2, v3(0.1, 0.8, 0.9)); 
        scene.addMesh(obj2);

        detector.addScene(scene);
    }
}