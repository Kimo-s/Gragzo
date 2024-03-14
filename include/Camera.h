#pragma once


#include <gragzo.hpp>
#include <glm/vec4.hpp>
#include <glm/common.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>


using namespace glm;

namespace gra{

    class Camera {
        public:
        glm::vec3 worldPosition, viewDirection, upDirection;
        float roll, pitch, yaw;
        float fov, farPlane, nearPlane;
        unsigned int width, height;
        float distanceToRotationCenter;

        float rotationSensitivity;
        bool firstClickRot = false;
        bool firstClickTranslation = false;

        double curScreenX, curScreenY;
        double curTranslationX, curTranslationY;


        Camera(glm::vec3 worldPosition, glm::vec3 viewDirection, glm::vec3 upDirection, unsigned int width, unsigned int height, float farplane, float nearplane, float fov){
            this->worldPosition = worldPosition;
            this->viewDirection = glm::normalize(viewDirection);
            this->upDirection = glm::normalize(upDirection);
            this->farPlane = 300.0f;
            this->nearPlane = 0.1f;
            this->width = width;
            this->height = height;
            this->fov = fov;
            this->upDirection = this->upDirection - this->viewDirection*glm::dot(this->viewDirection, this->upDirection);

            distanceToRotationCenter = 10.0f;
            rotationSensitivity = 130.0f;

            std::cout << farPlane << std::endl;

            this->curScreenX = 0.0f;
            this->curScreenY = 0.0f;
            this->curTranslationX = 0.0f;
            this->curTranslationY = 0.0f;
            this->roll = 0.0f;
            this->pitch = 0.0f;
            this->yaw = 0.0f;
        }

        void rotAroundCenter(float newMouseX, float newMouseY){
            float normalMouseX = (newMouseX - height/2) / height;
            float normalMouseY = (newMouseY - width/2) / width;
            float rotX = (normalMouseY - curScreenY)*rotationSensitivity;
            float rotY = (normalMouseX - curScreenX)*rotationSensitivity;

            glm::vec3 rotCenter = viewDirection*distanceToRotationCenter + worldPosition;
            glm::vec3 rotCenterView = -viewDirection;

            // Calculates upcoming vertical change in the Orientation
            glm::vec3 newOrientation = glm::rotate(rotCenterView, glm::radians(rotX), glm::normalize(glm::cross(rotCenterView, upDirection)));

            // Decides whether or not the next vertical Orientation is legal or not
            if (abs(glm::angle(newOrientation, upDirection) - glm::radians(90.0f)) <= glm::radians(85.0f))
            {
                rotCenterView = newOrientation;
            }
            // Rotates the Orientation left and right
            rotCenterView = glm::rotate(rotCenterView, glm::radians(-rotY), upDirection);

            worldPosition = rotCenterView*distanceToRotationCenter + rotCenter;
            viewDirection = -rotCenterView;
            curScreenX = normalMouseX;
            curScreenY = normalMouseY;
        }


        void translatePos(float newMouseX, float newMouseY){
            float normalMouseX = (newMouseX - height/2) / height;
            float normalMouseY = (newMouseY - width/2) / width;
            float translateX = (normalMouseX - curScreenX);    
            float translateY = (normalMouseY - curScreenY);

            glm::vec3 biTanget = glm::normalize(glm::cross(viewDirection, upDirection));
            glm::vec3 normal = glm::normalize(glm::cross(biTanget, viewDirection));

            worldPosition += normal * translateY - biTanget * translateX;

            curScreenX = normalMouseX;
            curScreenY = normalMouseY;
        }


        void setShaderValues(Shader* shader){
            glm::mat4 projMat = glm::perspective(glm::radians(fov), (float)width / (float)height, nearPlane, farPlane);
            shader->setMat4("projection", projMat);

            glm::mat4 viewMat = glm::lookAt(worldPosition, worldPosition + viewDirection, upDirection);
            shader->setMat4("view", viewMat);
        }

        void proccessWindow(GLFWwindow* window){
            if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS){
                double newMouseX, newMouseY;
                glfwGetCursorPos(window, &newMouseX, &newMouseY);
                if(firstClickTranslation == false){
                    firstClickTranslation = true;
                    float normalMouseX = (newMouseX - this->height/2) / this->height;
                    float normalMouseY = (newMouseY - this->width/2) / this->width;
                    this->curScreenX = normalMouseX;
                    this->curScreenY = normalMouseY;
                }
                this->translatePos(newMouseX, newMouseY);


            } else {
                if(firstClickTranslation == true){
                    firstClickTranslation = false;
                }   
                if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
                {

                    double newMouseX, newMouseY;
                    glfwGetCursorPos(window, &newMouseX, &newMouseY);
                    if(firstClickRot == false){
                        firstClickRot = true;
                        float normalMouseX = (newMouseX - this->height/2) / this->height;
                        float normalMouseY = (newMouseY - this->width/2) / this->width;
                        this->curScreenX = normalMouseX;
                        this->curScreenY = normalMouseY;
                    }
                    this->rotAroundCenter(newMouseX, newMouseY);

                } else if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE){
                    if(firstClickRot == true){
                        firstClickRot = false;
                    }
                }
            }


            // if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS){
            //     this->worldPosition.y += 0.1f;
            // }

            // if(glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS){
            //     this->worldPosition.y -= 0.1f;
            // }

            glm::vec3 biTanget = glm::normalize(glm::cross(viewDirection, upDirection));
            if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
                this->worldPosition += 0.1f*biTanget;
            }
            if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
                this->worldPosition -= 0.1f*biTanget;
            }
            if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
                this->worldPosition += 0.1f*viewDirection;
            }
            if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
                this->worldPosition -= 0.1f*viewDirection;
            }
        }


    };



}


