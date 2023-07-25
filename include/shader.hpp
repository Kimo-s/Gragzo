#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

struct Vertex{
    glm::vec3 pos;
    glm::vec3 col;
};

class Shader{
public:
    GLuint ID;

    Shader(const char* vertexPath, const char* fragmentPath){
        std::string vertexcode;
        std::string fragmentcode;

        std::ifstream vertexShaderFile;
        std::ifstream fragmentShaderFile;
        vertexShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
        fragmentShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);

        try{
            vertexShaderFile.open(vertexPath);
            fragmentShaderFile.open(fragmentPath);
            std::stringstream vShaderStream, fShaderStream;

            vShaderStream << vertexShaderFile.rdbuf();
            fShaderStream << fragmentShaderFile.rdbuf();

            vertexShaderFile.close();
            fragmentShaderFile.close();

            vertexcode = vShaderStream.str();
            fragmentcode = fShaderStream.str();

            // printf("Vert shader code:\n %s\n", vertexcode.c_str());

        } catch (std::ifstream::failure& e){
            std::cout << "ERROR: Enocountered error while reading the vertex or frament shader files: " << e.what() << std::endl;
        }



        const char* vShaderCode = vertexcode.c_str();
        const char* fShaderCode = fragmentcode.c_str();
        unsigned int vertexID, fragmentID;

        vertexID = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexID, 1, &vShaderCode, NULL);
        glCompileShader(vertexID);
        printf("Reading shaders...\n");
        checkCompileErrors(vertexID, "VERTEX");

        fragmentID = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentID, 1, &fShaderCode, NULL);
        glCompileShader(fragmentID);
        checkCompileErrors(fragmentID, "FRAGMENT");
        
        ID = glCreateProgram();
        glAttachShader(ID, vertexID);
        glAttachShader(ID, fragmentID);
        glLinkProgram(ID);
        checkCompileErrors(ID, "PROGRAM");
        
        glDeleteShader(vertexID);
        glDeleteShader(fragmentID);

    }

    
    void setMat4(const char *name, glm::mat4 &mat) {
        glUniformMatrix4fv(glGetUniformLocation(this->ID, name), 1, false, glm::value_ptr(mat));
    }

    void use(){
        glUseProgram(ID);
    }

    void terminate(){
        glDeleteProgram(ID);
    }


    // void uniform(const std::string &name, )

private:
    void checkCompileErrors(unsigned int shader, std::string shaderType){
        GLint check;
        GLchar info[1024];
        if(shaderType != "PROGRAM"){
            glGetShaderiv(shader, GL_COMPILE_STATUS, &check);
            if(!check){
                glGetShaderInfoLog(shader, 1024, NULL, info);
                std::cout << "ERROR: Shader failed to compile... " << shaderType << "\n" << info;
            }
        } else {
            glGetProgramiv(shader, GL_LINK_STATUS, &check);
            if(!check){
                glGetProgramInfoLog(shader, 1024, NULL, info);
                std::cout << "ERROR: Program failed to compile... " << shaderType << "\n" << info;
            }
        }
    }
};