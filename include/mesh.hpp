#pragma once
#include <vector>
#include <glad/glad.h>
#include <unordered_map>
#include <utility>
#include <bits/stdc++.h>
#include <glm/vec3.hpp>
#include <shader.hpp>
#include <Texture.hpp>

namespace gra{
    struct Vertex{
        glm::vec3 pos;
        glm::vec3 normal;
        glm::vec2 uv;
    };


    struct Triangle{
        uint32_t inds[3];
    };

    class Mesh {
        public:

        Mesh(){}

        Mesh(const char* filename, Shader &shader);

        ~Mesh();

        void setupMesh();

        Mesh(std::vector<Vertex>& verts, std::vector<uint>& indices){
            this->verts = verts;
            numOfVerts = verts.size();

            numOfTrians = indices.size()/3;
            std::cout << "numoftrians: " << numOfTrians << "\n";
            for(int i = 0; i < numOfTrians; i++){  
                Triangle trian;
                trian.inds[0] =  indices[i*3];
                trian.inds[1] =  indices[i*3+1];
                trian.inds[2] =  indices[i*3+2];

                if(trian.inds[0] >= numOfVerts || trian.inds[1] >= numOfVerts || trian.inds[2] >= numOfVerts){
                    std::cout << "ERROR: Got index number bigger than number of vertices.\n";
                }
                triangleArr.push_back(trian);
            }

            setupMesh();
        }

        void setColor(glm::vec4 col){
            specularTexture = new Texture<unsigned char>(glm::vec4(col.x, col.y, col.z, 1.0));
        }

        void loadTexture(const char* filepath){
            specularTexture = new Texture<unsigned char>(filepath);
        }

        void draw() {
            shaderProgram->use();
            shaderProgram->setMat4("model", modelMat);

            if(specularTexture){
                glActiveTexture(GL_TEXTURE0);
                shaderProgram->setInt("specTex", 0);
                glBindTexture(GL_TEXTURE_2D, specularTexture->texture);
            }


            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, numOfTrians*3, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);

            for(int i = 0; i < debugMeshes.size(); i++){
                debugMeshes[i].get()->draw();
            }
        }

        void updateBuffers();

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


        // Gets a model mat that rotates an object from vector a to match at b and translation to center
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

        virtual glm::vec3 support(glm::vec3 direction){
            glm::vec3 v = verts[0].pos;
            float maxDot = glm::dot(v, direction);
            for(int i = 0; i < verts.size(); i++){
                float d = glm::dot(verts[i].pos, direction);
                if(d >= maxDot){
                    v = verts[i].pos;
                    maxDot = d;
                }
            }
            return v;
        }



        GLuint VAO, VBO, EBO;
        Texture<unsigned char>* specularTexture;
        Shader *shaderProgram;
        size_t numOfVerts, numOfTrians;
        glm::mat4 modelMat = glm::mat4(1.0f);
        std::vector<Vertex> verts;
        std::vector<Triangle> triangleArr;
        std::vector<Vertex> renderVerts;
        std::vector<unsigned int> renderTriangles;
        glm::vec3 center;
        std::vector<std::shared_ptr<Mesh>> debugMeshes;
    };




    class PhysicalObject : public Mesh{
        public:
        PhysicalObject(){}
        PhysicalObject(const char* filename, Shader& shader) : Mesh(filename, shader){};
        PhysicalObject(std::vector<Vertex>& verts, std::vector<uint>& indices) : Mesh(verts, indices){
        }
        // ~PhysicalObject(){}

        virtual void simulateTimeStep(float dt) = 0;

    };
}

