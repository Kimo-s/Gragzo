#pragma once
#include <mesh.hpp>
#include <vector>
#include <shader.hpp>



class SoftBody : public PhysicalObject {
    public:

    SoftBody(const char* filename, Shader& shader);

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
    float particalMass = 1.0f;
    std::unordered_map<uint, std::unordered_map<uint, float>> edgeConnections;
    
};