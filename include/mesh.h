#pragma once
#include <vector>
#include <glad/glad.h>
#include <glm/vec3.hpp>
#include <unordered_map>
#include <utility>
#include <bits/stdc++.h>



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

    Mesh();

    Mesh(const char* filename);

    ~Mesh();

    void setupMesh();

    void draw() {
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, numOfVerts, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    void updateBuffers();

    // uint getVertIndex(Vertex v){
    //     verts.
    // }



    GLuint VAO, VBO, EBO;
    size_t numOfVerts, numOfTrians;
    std::vector<Vertex> verts;
    std::vector<Triangle> triangleArr;

};

class PhysicalObject : public Mesh{
    public:
    PhysicalObject(){}
    PhysicalObject(const char* filename) : Mesh(filename){};
    // ~PhysicalObject(){}


    virtual void simulateTimeStep(float dt) = 0;

};

class SoftBody : public PhysicalObject {
    public:

    // SoftBody(Mesh& bodmesh){
    //     this->mesh = &bodmesh;

    //     vertsN = mesh->numOfVerts;
    //     velocities = new glm::vec3[vertsN];
    //     for(int i = 0; i < vertsN; i++){
    //         velocities[i] = glm::vec3(0.0, 0.0, 0.0);
    //     }

    //     for(int t = 0; t < mesh->numOfTrians; t++){
    //         uint v1 = mesh->triangleArr[t].inds[0];
    //         uint v2 = mesh->triangleArr[t].inds[1];
    //         uint v3 = mesh->triangleArr[t].inds[2];
            

    //         if(edgeConnections.count(std::make_pair(v1, v2)) == 0 || edgeConnections.count(std::make_pair(v2, v1)) == 0 ){
    //             edgeConnections[std::make_pair(v1, v2)] = 1.0f;
    //         }

    //         // edgeConnections[std::make_pair(v1, v2)] = 1.0; //glm::length(mesh->verts[v1].pos - mesh->verts[v2].pos);
    //         // edgeConnections[std::make_pair(v1, v3)] = 1.0; //glm::length(mesh->verts[v1].pos - mesh->verts[v3].pos);
    //         // edgeConnections[std::make_pair(v3, v2)] = 1.0; //glm::length(mesh->verts[v3].pos - mesh->verts[v2].pos);
    //         // std::cout << "Testing";
    //     }
    //     std::cout << "Loaded soft body\n";
    // }

    SoftBody(const char* filename) : PhysicalObject(filename) {

        vertsN = numOfVerts;
        velocities = new glm::vec3[vertsN];
        for(int i = 0; i < vertsN; i++){
            velocities[i] = glm::vec3(0.0, 0.0, 0.0);
        }

        for(int t = 0; t < numOfTrians; t++){
            uint v1 = triangleArr[t].inds[0];
            uint v2 = triangleArr[t].inds[1];
            uint v3 = triangleArr[t].inds[2];
            

            // if(edgeConnections.count(std::make_pair(v1, v2)) == 0 || edgeConnections.count(std::make_pair(v2, v1)) == 0 ){
            //     edgeConnections[std::make_pair(v1, v2)] = 1.0f;
            // }

            // edgeConnections[std::make_pair(v1, v2)] = 1.0; //glm::length(mesh->verts[v1].pos - mesh->verts[v2].pos);
            // edgeConnections[std::make_pair(v1, v3)] = 1.0; //glm::length(mesh->verts[v1].pos - mesh->verts[v3].pos);
            // edgeConnections[std::make_pair(v3, v2)] = 1.0; //glm::length(mesh->verts[v3].pos - mesh->verts[v2].pos);
            // std::cout << "Testing";
        }
        std::cout << "Loaded soft body\n";
    }

    void simulateTimeStep(float dt) override {
        glm::vec3 *prevPos;
        prevPos = new glm::vec3[vertsN];

        for(int i = 0; i < vertsN; i++){
            velocities[i] += dt*glm::vec3(0.0,0.0,-9.8);
            prevPos[i] = verts[i].pos;
            verts[i].pos += dt*velocities[i];

            if(verts[i].pos.z < -1.0){
                verts[i].pos.z = 0.1;
            }

        }

        updateBuffers();
    };


    uint vertsN;
    glm::vec3 *velocities;
    // Mesh* mesh;
    float particalMass;
    std::unordered_map<std::pair<uint, uint>, float, hash_indicies> edgeConnections;
    
};



