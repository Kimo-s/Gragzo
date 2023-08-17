#pragma once
#include <vector>
#include <glad/glad.h>
#include <unordered_map>
#include <utility>
#include <bits/stdc++.h>
#include <glm/vec3.hpp>



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

    Mesh(){};

    Mesh(const char* filename);

    ~Mesh();

    void setupMesh();

    void draw() {
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
    size_t numOfVerts, numOfTrians;
    std::vector<Vertex> verts;
    std::vector<Triangle> triangleArr;
    std::vector<Triangle> connectivity;
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

    SoftBody(const char* filename);

    void initSoftBody();

    SoftBody(std::vector<Vertex>& verts, std::vector<uint>& indices){
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

        initSoftBody();
    }

    void simulateTimeStep(float dt) override;

    void printEdges(){
        for(int i = 0; i < numOfVerts; i++){
            std::cout << "Size of vertex [" << i << "]: ";
            std::unordered_map<uint, float>::iterator itr;
            for(itr = edgeConnections[i].begin(); itr != edgeConnections[i].end(); itr++){
                std::cout << "{" << itr->first << ", " << itr->second << "}, ";
            }
            std::cout << "\n";
        }
    }


    glm::vec3 *velocities;
    // Mesh* mesh;
    float particalMass = 1000.0f;
    std::unordered_map<uint, std::unordered_map<uint, float>> edgeConnections;
    
};



