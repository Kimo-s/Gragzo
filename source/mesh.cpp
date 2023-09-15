#define TINYOBJLOADER_IMPLEMENTATION


#include <mesh.hpp>
#include <SoftBody.hpp>
#include <gragzo.hpp>
#include "OBJ_Loader.h"
#include "tiny_obj_loader.h"
#include "gui_parameters.hpp"
#include <glm/vec3.hpp>
#include <glm/common.hpp>
#include <cassert>

void distanceConstraintSolve(glm::vec3& x1, glm::vec3& x2, float& lagrange, float compliance, float m, float l0, float dt){
    
    glm::vec3 dir = x1 - x2;
    float c = glm::length(dir) - l0;

    float w = 1.0/m;

    float alpha = compliance/(dt*dt);
    float delta_lagrange = (-c - lagrange * alpha) / (2.0*w + alpha);
    glm::vec3 v = delta_lagrange * dir / (glm::length(dir) + 1e-5f);
    lagrange += delta_lagrange;

    x1 += w*v;
    x2 += -w*v;
}

void jointDistanceConstraint(SoftBody& obj, float dt, int solverIterations, float stif){

    for(int i = 0; i < obj.numOfVerts; i++){
        uint x1 = i;

        std::unordered_map<uint, float>::iterator itr;
        for(itr = obj.edgeConnections[i].begin(); itr != obj.edgeConnections[i].end(); itr++){
            float lagrange = 0.0;
            float l0 = itr->second;
            uint x2 = itr->first;
            for(int s = 0; s < solverIterations; s++){
                distanceConstraintSolve(obj.verts[x1].pos, obj.verts[x2].pos, lagrange, stif, obj.particalMass, l0, dt);    
            }

        }

    }

}

Mesh::Mesh(const char* filename) {
    // objl::Loader loader;
    // if(!loader.LoadFile(filename)){
    //     std::cerr << "Error: unable to load the file\n";
    // }

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    std::string warn;
    std::string err;

    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename);

    if (!warn.empty()) {
        std::cout << warn << std::endl;
    }

    if (!err.empty()) {
        std::cerr << err << std::endl;
    }

    if(!ret){
        std::cerr << "Error: unable to load the file\n";
    }


    std::cout << "size of verts: " << attrib.vertices.size() << std::endl;
    // numOfVerts = shapes[0].mesh.num_face_vertices.size() * 3;

    verts.resize(attrib.vertices.size()/3);

    // Loop over shapes
    // Loop over faces(polygon)
    size_t index_offset = 0;
    for (size_t f = 0; f < shapes[0].mesh.num_face_vertices.size(); f++) {
        size_t fv = size_t(shapes[0].mesh.num_face_vertices[f]);

        Triangle trian;

        // Loop over vertices in the face.
        for (size_t v = 0; v < fv; v++) {
            // access to vertex
            tinyobj::index_t idx = shapes[0].mesh.indices[index_offset + v];


            trian.inds[v] = size_t(idx.vertex_index);

            tinyobj::real_t vx = attrib.vertices[3*size_t(idx.vertex_index)+0];
            tinyobj::real_t vy = attrib.vertices[3*size_t(idx.vertex_index)+1];
            tinyobj::real_t vz = attrib.vertices[3*size_t(idx.vertex_index)+2];
            verts[size_t(idx.vertex_index)].pos = glm::vec3(vx, vy, vz);

            if (idx.normal_index >= 0) {
                tinyobj::real_t nx = attrib.normals[3*size_t(idx.normal_index)+0];
                tinyobj::real_t ny = attrib.normals[3*size_t(idx.normal_index)+1];
                tinyobj::real_t nz = attrib.normals[3*size_t(idx.normal_index)+2];
                verts[size_t(idx.vertex_index)].normal = glm::vec3(nx, ny, nz);
            } else {
                verts[size_t(idx.vertex_index)].normal = glm::vec3(1.0, 0.2, 0.0);
            }

            verts[size_t(idx.vertex_index)].col = glm::vec3(1.0, 0.0, 0.0);


        }
        triangleArr.push_back(trian);

        index_offset += fv;
    }


    numOfTrians = triangleArr.size();
    numOfVerts = verts.size();

    std::cout << "Loaded mesh from " << filename 
              << " [Number of vertices: " << numOfVerts << "]"
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
    std::vector<unsigned int> indices;
    for(int i = 0; i < triangleArr.size(); i++){
        indices.push_back(triangleArr[i].inds[0]);
        indices.push_back(triangleArr[i].inds[1]);
        indices.push_back(triangleArr[i].inds[2]);
    }
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), 
                 &indices[0], GL_STATIC_DRAW);
                 
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


SoftBody::SoftBody(const char* filename) : PhysicalObject(filename) {
    // printTriangleArray();
    initSoftBody();
}

void SoftBody::initSoftBody() {
    velocities = new glm::vec3[numOfVerts];
    for(int i = 0; i < numOfVerts; i++){
        velocities[i] = glm::vec3(0.0, 0.0, 0.0);
    }

    edgeConnections.reserve(numOfVerts);

    for(int t = 0; t < numOfTrians; t++){
        uint v1 = triangleArr[t].inds[0];
        uint v2 = triangleArr[t].inds[1];
        uint v3 = triangleArr[t].inds[2];
        
        glm::vec3 vec = verts[v1].pos - verts[v2].pos;
        float dist = glm::length(vec);
        if(edgeConnections[v2].count(v1) == 0){
            edgeConnections[v1][v2] = dist;
        }

        dist = glm::length(verts[v1].pos - verts[v3].pos);
        if(edgeConnections[v3].count(v1) == 0){
            edgeConnections[v1][v3] = dist;
        }

        dist = glm::length(verts[v2].pos - verts[v3].pos);
        if(edgeConnections[v3].count(v2) == 0){
            edgeConnections[v2][v3] = dist;
        }
        // this->verts[v1].col = glm::vec3(0.0,1.0,0.0);
        // this->verts[v2].col = glm::vec3(0.0,1.0,0.0);
        // this->verts[v3].col = glm::vec3(0.0,1.0,0.0);
        // std::cout << v3 << "\n";
    }

    // std::cout << " [Number of vertices: " << numOfVerts << "]"
    //           << " [Number of triangles: " << numOfTrians << "]" << std::endl;
    std::cout << "Loaded soft body edge size: " << edgeConnections[3].size() << "\n";
    // printEdges();
    // printTriangleArray();
    // printVertices();
}


void SoftBody::simulateTimeStep(float dt) {
    glm::vec3 *prevPos;
    prevPos = new glm::vec3[numOfVerts];
    int solveriterations = 1;
    int subiterations = 200;
    float dts = dt/subiterations;


    // float stifness = 1.0 - powf((1.0 - 0.3), 1.0/(1.0*n));
    float stifness = 0.0;

    for(int subiter = 0; subiter < subiterations; subiter++){

        for(int i = 0; i < numOfVerts; i++){
            velocities[i] += dts*glm::vec3(0.0,0.0,-9.8);
            prevPos[i] = verts[i].pos;
            verts[i].pos += dts*velocities[i];


            glm::vec3 n(1.0, 0.0, 1.0);
            // if(glm::dot(verts[i].pos, n) < 0.0){
            //     verts[i].pos = prevPos[i];
            //     prevPos[i] = verts[i].pos;
            // }

            if(verts[i].pos.z < -2.0){
                verts[i].pos = prevPos[i];
                // prevPos[i].z = -2.0;
                // verts[i].pos.z = -2.0;
            }

        }

        jointDistanceConstraint(*this, dts, solveriterations, guiParameters.complaince);

        for(int i = 0; i < numOfVerts; i++){
            glm::vec3 vel = (verts[i].pos - prevPos[i])/dts; 

            if(isnan(glm::length(verts[i].pos - prevPos[i]))){
                std::cout << "Nan velocity!\n";
                exit(1);       
            }
            
        }

    }




    updateBuffers();
};