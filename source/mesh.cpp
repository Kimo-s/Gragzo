#include <mesh.h>
#include "OBJ_Loader.h"
#include <iostream>

void jointDistanceConstraint(SoftBody obj, float dt){

    for(int i = 0; i < obj.edgeConnections.size(); i++){



    }

}

Mesh::Mesh(const char* filename) {
    objl::Loader loader;
    if(!loader.LoadFile(filename)){
        std::cerr << "Error: unable to load the file\n";
    }


    objl::Mesh mesh = loader.LoadedMeshes[0];
    numOfVerts = mesh.Vertices.size();
    for(int i = 0; i < numOfVerts; i++){
        Vertex vert;
        objl::Vertex vec = mesh.Vertices[i];
        vert.pos = glm::vec3(vec.Position.X, vec.Position.Y, vec.Position.Z);
        vert.normal = glm::vec3(vec.Normal.X, vec.Normal.Y, vec.Normal.Z);
        vert.col = glm::vec3(1.0, 0.3, 0.0);

        verts.push_back(vert);
    }

    numOfTrians = mesh.Indices.size()/3;
    for(int i = 0; i < numOfTrians; i++){  
        Triangle trian;
        trian.inds[0] =  mesh.Indices[i*3];
        trian.inds[1] =  mesh.Indices[i*3+1];
        trian.inds[2] =  mesh.Indices[i*3+2];
        triangleArr.push_back(trian);
    }

    std::cout << "Loaded mesh from " << filename 
              << " [Number of Vertices: " << numOfVerts << "]"
              << " [Number of triangles: " << numOfTrians << "]" << std::endl;


    setupMesh();
}

Mesh::~Mesh(){
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}


void Mesh::setupMesh(){
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER ,VBO);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(Vertex), &verts[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER ,EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, triangleArr.size() * sizeof(Triangle), 
                 &triangleArr[0], GL_STATIC_DRAW);
                 
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, col)); 
    
    glBindVertexArray(0);
}

void Mesh::updateBuffers(){
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(Vertex), &verts[0], GL_STATIC_DRAW);
}