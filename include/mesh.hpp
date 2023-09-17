#pragma once
#include <vector>
#include <glad/glad.h>
#include <unordered_map>
#include <utility>
#include <bits/stdc++.h>
#include <glm/vec3.hpp>
#include <shader.hpp>



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

    void draw() {
        shaderProgram->use();
        shaderProgram->setMat4("model", modelMat);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, numOfTrians*3, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
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




    GLuint VAO, VBO, EBO;
    Shader *shaderProgram;
    size_t numOfVerts, numOfTrians;
    glm::mat4 modelMat = glm::mat4(1.0f);
    std::vector<Vertex> verts;
    std::vector<Triangle> triangleArr;
    std::vector<Triangle> connectivity;
};




class PhysicalObject : public Mesh{
    public:
    PhysicalObject(){}
    PhysicalObject(const char* filename, Shader& shader) : Mesh(filename, shader){};
    // ~PhysicalObject(){}


    virtual void simulateTimeStep(float dt) = 0;

};