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
#include <sstream>

using namespace gra;
gui_parameters guiParameters;

const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 600;
float targetFPS = 1/60.0;

glm::vec3 Position = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 Orientation = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);

float speed = 0.1f;
float sensitivity = 1.0f;

Scene scene;
gra::CollisionDetector detector;
Shader* shader;


bool firstClick = true;

bool leftMouseHeld = false;

float Yaw   = -90.0f;
float Pitch = 0.0f;
float lastX = WIDTH / 2.0f;
float lastY = HEIGHT / 2.0f;

void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void processInput(GLFWwindow *window);

void guiSetup(GLFWwindow* window, ImGuiIO& io){
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();


    ImGui::Begin("Controls");

    ImGui::SliderFloat("Complaince", &guiParameters.complaince, 0.0f, 3.2f); 


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

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);;

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

    std::shared_ptr<RigidBody> obj = std::make_shared<RigidBody>("../sphere.obj", *shader);
    // std::shared_ptr<RigidBody> obj2 = std::make_shared<RigidBody>("../sphere.obj", *shader);
    obj->x = glm::vec3(0.0,0.0,0.0);
    // obj2->x = glm::vec3(-2.0,0.0,0.0);

    scene.addMesh(obj);
    // scene.addMesh(obj2);

    detector.addScene(scene);


    // SoftBody obj(vertices, indices);


    glEnable(GL_DEPTH_TEST);

    

    // This is the render loop
    // float curTime = glfwGetTime();
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

            processInput(window);


            glm::mat4 projMat = glm::perspective(glm::radians(60.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 300.0f);
            shader->setMat4("projection", projMat);

            glm::mat4 viewMat = glm::lookAt(Position, Position + Orientation, Up);
            shader->setMat4("view", viewMat);

            timeSum = 0;
            
            scene.simulatePhysics(targetFPS);
            scene.drawScene();

            detector.detectCollisions();

            // float fps = 1.0/delta;
            // std::stringstream ss;
            // ss << "Gragzp (fps:" << fps << ")";

            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            // glfwSetWindowTitle(window, ss.str().c_str());
            glfwSwapBuffers(window);
            glfwPollEvents();
        }

    }

    shader->terminate();

    glfwTerminate();
    return 0;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    // if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE){
    //     leftMouseHeld = false;
    //     return;
    // }

    // xpos = static_cast<float>(xpos);
    // ypos = static_cast<float>(ypos);


    // if(leftMouseHeld) {
    //     leftMouseHeld = false;
    //     lastX = xpos;
    //     lastY = ypos;
    // }


    // float xoffset = xpos - lastX;
    // float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    // lastX = xpos;
    // lastY = ypos;

    // xoffset *= 0.02;
    // yoffset *= 0.02;



    // if(Pitch > 89.0f)
    //     Pitch = 89.0f;
    // if(Pitch < -89.0f)
    //     Pitch = -89.0f;


    // Yaw   += xoffset;
    // Pitch += yoffset;

    // // // make sure that when pitch is out of bounds, screen doesn't get flipped
    // if (Pitch > 89.0f)
    //     Pitch = 89.0f;
    // if (Pitch < -89.0f)
    //     Pitch = -89.0f;

    // float camposr = glm::length(cameraPos);

    // glm::vec3 direction;
    // direction.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    // direction.y = sin(glm::radians(Pitch));
    // direction.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    // cameraFront = glm::normalize(direction);

    // glfwSetCursorPos(window, HEIGHT/2, WIDTH/2);


    // glm::vec3 right = glm::normalize(glm::cross(cameraFront, cameraUp));
    // cameraUp = glm::normalize(glm::cross(right, cameraFront));
}



void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    Position = (glm::length(Position) + static_cast<float>(-yoffset) * 0.4f) * glm::normalize(Position);
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

   if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		Position += speed * Orientation;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		Position += speed * -glm::normalize(glm::cross(Orientation, Up));
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		Position += speed * -Orientation;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		Position += speed * glm::normalize(glm::cross(Orientation, Up));
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		Position += speed * Up;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		Position += speed * -Up;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		speed = 0.4f;
	}
	else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
	{
		speed = 0.1f;
	}

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		// Hides mouse cursor
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

		// Prevents camera from jumping on the first click
		if (firstClick)
		{
			glfwSetCursorPos(window, (WIDTH / 2), (HEIGHT / 2));
			firstClick = false;
		}

		// Stores the coordinates of the cursor
		double mouseX;
		double mouseY;
		// Fetches the coordinates of the cursor
		glfwGetCursorPos(window, &mouseX, &mouseY);

		// Normalizes and shifts the coordinates of the cursor such that they begin in the middle of the screen
		// and then "transforms" them into degrees 
		float rotX = sensitivity * (float)(mouseY - (HEIGHT / 2)) / HEIGHT;
		float rotY = sensitivity * (float)(mouseX - (WIDTH / 2)) / WIDTH;

		// Calculates upcoming vertical change in the Orientation
		glm::vec3 newOrientation = glm::rotate(Orientation, glm::radians(-rotX), glm::normalize(glm::cross(Orientation, Up)));

		// Decides whether or not the next vertical Orientation is legal or not
		if (abs(glm::angle(newOrientation, Up) - glm::radians(90.0f)) <= glm::radians(85.0f))
		{
			Orientation = newOrientation;
		}

		// Rotates the Orientation left and right
		Orientation = glm::rotate(Orientation, glm::radians(-rotY), Up);

		// Sets mouse cursor to the middle of the screen so that it doesn't end up roaming around
		glfwSetCursorPos(window, (WIDTH / 2), (HEIGHT / 2));
	}
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
	{
		// Unhides cursor since camera is not looking around anymore
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		// Makes sure the next time the camera looks around it doesn't jump
		firstClick = true;
	}
}


void mouse_button_callback(GLFWwindow* window, int button, int action, int mods){
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS){

        std::shared_ptr<RigidBody> obj2 = std::make_shared<RigidBody>("../sphere.obj", *shader);
        obj2->x = Position + glm::normalize(Orientation);
        obj2->v = glm::normalize(Orientation)*8.0f;
        scene.addMesh(obj2);

        detector.addScene(scene);
    }
}