#pragma once
#include "RigidBody.hpp"
#include "scene.hpp"

namespace gra {
    // glm::vec3 tripleProduct(glm::vec3 a, glm::vec3 b, glm::vec3 c);
    using v3 = glm::vec3;

    struct CollisionInfo {
        v3 contactA;
        v3 contactB;
        float penetration;
        v3 normal;
    };


    class CollisionDetector {
        public:

        std::vector<std::shared_ptr<RigidBody> > meshes;


        void setColor(std::shared_ptr<Mesh> mesh, v3 col){
            for(int i = 0; i < mesh->numOfVerts; i++){
                mesh->verts[i].col = col;
            }
            mesh->updateBuffers();
        }

        CollisionDetector(){}

        CollisionDetector(Scene& scene){
            for(std::shared_ptr<RigidBody> mesh: scene.meshes){
                meshes.push_back(mesh);
            }
        }

        void addScene(Scene& scene){
            meshes.clear();
            for(std::shared_ptr<RigidBody> mesh: scene.meshes){
                meshes.push_back(mesh);
            }
        }

        void detectCollisions();

        void updateMeshMotion(RigidBody::Ptr a, RigidBody::Ptr b, CollisionInfo& info);


        // static void checkCollision(RigidBody* a, RigidBody* b){
        //     bool foundIntersection = false;
        //     glm::vec3 centerOfB(0.0f);
        //     for(int i = 0; i < b->numOfVerts; i++){
        //         centerOfB += b->verts[i].pos;
        //     }
        //     centerOfB /= b->numOfVerts;

        //     for(int x = 0; x < a->numOfTrians; x++){
        //         for(int c = 0; c < b->numOfVerts; c++){
        //             glm::vec3 A = a->modelMat*glm::vec4(a->verts[a->triangleArr[x].inds[0]].pos, 1.0f);
        //             glm::vec3 B = a->modelMat*glm::vec4(a->verts[a->triangleArr[x].inds[1]].pos, 1.0f);
        //             glm::vec3 C = a->modelMat*glm::vec4(a->verts[a->triangleArr[x].inds[2]].pos, 1.0f);

        //             glm::vec3 vertex = b->modelMat*glm::vec4(b->verts[c].pos, 1.0f);
                    
        //             glm::vec3 centerOfTriangle = (A+B+C)/3.0f;
        //             glm::vec3 normalOfTriangle = glm::normalize(glm::cross(B-A, C-A));

        //             float t = glm::length(centerOfTriangle-vertex);

        //             if(t > 3.5f){
        //                 continue;
        //             }

        //             if(glm::dot(normalOfTriangle, centerOfB - centerOfTriangle) > 0.0){
        //                 normalOfTriangle = -normalOfTriangle;
        //             }
                    

        //             // if(rayTriangleIntersect(vertex, glm::normalize(centerOfTriangle-vertex), A, B, C, t)){
        //                 // std::cout << t << std::endl;


        //             if(t < 0.5f){
        //                 float dt = 0.016f;
        //                 glm::vec3 impluse = -6.0f*normalOfTriangle;
        //                 std::cout << t << std::endl;
                        
        //                 a->v += (impluse/a->mass) * dt;
        //                 b->v += -(impluse/b->mass) * dt;

        //                 glm::vec3 p = centerOfTriangle + normalOfTriangle * t;
        //                 glm::vec3 torque = glm::cross((p - a->x), impluse);

        //                 // a->L += -a->Iinv*torque;
        //                 // b->L += b->Iinv*torque;

        //                 foundIntersection = true;
        //                 break;
        //             }
        //             // }

        //         }
        //         if(foundIntersection){
        //             break;
        //         }
        //     }
        // }

        
    };




}