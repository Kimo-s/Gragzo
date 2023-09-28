#pragma once
#include <vector>
#include <glad/glad.h>
#include <unordered_map>
#include <utility>
#include <bits/stdc++.h>
#include <glm/vec3.hpp>
#include <shader.hpp>

namespace gra{
    struct Vertex{
    glm::vec3 pos;
    glm::vec3 col;
    glm::vec3 normal;
    };


    struct Triangle{
        uint32_t inds[3];
    };

    struct hash_indicies {

        template<class T1, class T2>
        size_t operator()(const std::pair<T1, T2>& p) const {
            auto hash1 = std::hash<T1>{}(p.first);
            auto hash2 = std::hash<T2>{}(p.second);

            if(hash1 != hash2){
                return hash1 ^ hash2;
            }

            return hash1;
        } 

    };


    class Mesh {
        public:

        Mesh(){}

        Mesh(const char* filename, Shader &shader);

        ~Mesh();

        void setupMesh();

        virtual glm::vec3 support(glm::vec3 direction){
            glm::mat3 rotMat(1.0f);
            rotMat[0][0] = modelMat[0][0];
            rotMat[0][1] = modelMat[0][1];
            rotMat[0][2] = modelMat[0][2];
            rotMat[1][0] = modelMat[1][0];
            rotMat[1][1] = modelMat[1][1];
            rotMat[1][2] = modelMat[1][2];
            rotMat[2][0] = modelMat[2][0];
            rotMat[2][1] = modelMat[2][1];
            rotMat[2][2] = modelMat[2][2];

            direction = glm::inverse(rotMat) * direction;

            glm::vec3 v = verts[0].pos;
            float maxDot = glm::dot(v, direction);
            for(int i = 1; i < numOfVerts; i++){
                if(glm::dot(verts[i].pos, direction) > glm::dot(v, direction)){
                    v = verts[i].pos;
                }
            }
            return rotMat * v + center;
        }

        void draw() {
            shaderProgram->use();
            shaderProgram->setMat4("model", modelMat);

            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, numOfTrians*3, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);

            for(int i = 0; i < debugMeshes.size(); i++){
                debugMeshes[i].get()->draw();
            }
        }

        void updateBuffers();

        // uint getVertIndex(Vertex v){
        //     verts.
        // }

        void printTriangleArray(){
            for(int i = 0; i < numOfTrians; i++){
                std::cout << "Triangle [" << i << "]: " << triangleArr[i].inds[0] << ", " << triangleArr[i].inds[1] << ", " << triangleArr[i].inds[2] ;
                std::cout << "\n";
            }
        }

        void printVertices(){
            for(int i = 0; i < numOfVerts; i++){
                std::cout << "Vertex pos [" << i << "]: " << verts[i].pos.x << ", " << verts[i].pos.y << ", " << verts[i].pos.z ;
                std::cout << "\n";
            }
        }

        void addDebugMeshs(std::shared_ptr<Mesh> debugMesh){
            debugMeshes.push_back(debugMesh);
        }

        void matchModelMatToVector(glm::vec3 a, glm::vec3 b, glm::vec3 center, float scale){
            b = glm::normalize(b);
            a = glm::normalize(a);
            float theta = acos(glm::dot(a,b));
            if(glm::isnan(theta)){
                modelMat = glm::mat4(1.0f);
                modelMat[3][0] = center.x;
                modelMat[3][1] = center.y;
                modelMat[3][2] = center.z;
                modelMat[3][3] = 1.0f;
                return;
            }
            glm::vec3 rotAxis = glm::cross(a,b);
            glm::quat q = glm::angleAxis(-theta, rotAxis);
            q = glm::normalize(q);

            modelMat = glm::mat4_cast(q);
            modelMat[3][0] = center.x;
            modelMat[3][1] = center.y;
            modelMat[3][2] = center.z;
            modelMat[3][3] = 1.0f;
        }




        GLuint VAO, VBO, EBO;
        Shader *shaderProgram;
        size_t numOfVerts, numOfTrians;
        glm::mat4 modelMat = glm::mat4(1.0f);
        std::vector<Vertex> verts;
        std::vector<Triangle> triangleArr;
        std::vector<Triangle> connectivity;
        glm::vec3 center;
        std::vector<std::shared_ptr<Mesh>> debugMeshes;
    };




    class PhysicalObject : public Mesh{
        public:
        PhysicalObject(){}
        PhysicalObject(const char* filename, Shader& shader) : Mesh(filename, shader){};
        // ~PhysicalObject(){}


        virtual void simulateTimeStep(float dt) = 0;

    };
}

