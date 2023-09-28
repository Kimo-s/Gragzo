#pragma once
#include "mesh.hpp"
#include <glm/gtc/quaternion.hpp>


namespace gra{
    class RigidBody : public PhysicalObject {
        public:
        using Ptr = std::shared_ptr<RigidBody>;
        using m3 = glm::mat3;
        using v3 = glm::vec3;

        float mass;
        m3 Ibody, Ibodyinv;

        v3 x;
        v3 P;
        glm::quat q;
        v3 L;

        m3 Iinv;
        v3 v;
        v3 omega;

        v3 F;
        v3 T;

        //Debug
        std::shared_ptr<Mesh> LArrow;


        RigidBody(const char* filename, Shader &shader);

        void initRigidBody();

        void simulateTimeStep(float dt) override;

        // glm::vec3 support(glm::vec3 direction) override{
        //     glm::vec3 v = modelMat*glm::vec4(verts[0].pos,1.0);
        //     for(int i = 1; i < numOfVerts; i++){
        //         if(glm::dot(verts[i].pos, direction) > glm::dot(v, direction)){
        //             v = modelMat*glm::vec4(verts[i].pos,1.0);
        //         }
        //     }
        //     return v;
        // }

        float calcuateTriangleVolume(v3 p1, v3 p2, v3 p3){
            float v321 = p3.x*p2.y*p1.z;
            float v231 = p2.x*p3.y*p1.z;
            float v312 = p3.x*p1.y*p2.z;
            float v132 = p1.x*p3.y*p2.z;
            float v213 = p2.x*p1.y*p3.z;
            float v123 = p1.x*p2.y*p3.z;
            return (1.0f/6.0f)*(-v321 + v231 + v312 - v132 - v213 + v123);
        }

        float calculateVolume(){
            float vol = 0;
            for(int i = 0; i < numOfTrians; i++){
                vol += calcuateTriangleVolume(verts[triangleArr[i].inds[0]].pos, verts[triangleArr[i].inds[1]].pos, verts[triangleArr[i].inds[2]].pos);
            }
            return fabs(vol);
        }

        v3 calculateCenterOfMass(){
            v3 centerOfMass(0.0f);
            float volume = 0;
            for(int i = 0; i < numOfTrians; i++){
                v3 a = verts[triangleArr[i].inds[0]].pos;
                v3 b = verts[triangleArr[i].inds[1]].pos;
                v3 c = verts[triangleArr[i].inds[2]].pos;
                float vol = signedTetrahedronVolume(a,b,c);
                centerOfMass += vol * (a+b+c) / 4.0f;
                volume += vol;
            }
            return centerOfMass/volume;
        }

        m3 claculateInertia(v3 centerOfMass){
            float volume = 0;
            v3 diag(0.0f);
            v3 offd(0.0f);
            for(int i = 0; i < numOfTrians; i++){
                v3 a = verts[triangleArr[i].inds[0]].pos;
                v3 b = verts[triangleArr[i].inds[1]].pos;
                v3 c = verts[triangleArr[i].inds[2]].pos;

                m3 A(a-centerOfMass, b-centerOfMass, c-centerOfMass);
                float d = glm::determinant(A);
                volume += d;

                for(int j = 0; j < 3; j++){
                    int j1=(j+1)%3;     
                    int j2=(j+2)%3;   
                    diag[j] += (A[0][j]*A[1][j] + A[1][j]*A[2][j] + A[2][j]*A[0][j] + 
                                A[0][j]*A[0][j] + A[1][j]*A[1][j] + A[2][j]*A[2][j]  ) *d; // divide by 60.0f later;
                    offd[j] += (A[0][j1]*A[1][j2]  + A[1][j1]*A[2][j2]  + A[2][j1]*A[0][j2]  +
                                A[0][j1]*A[2][j2]  + A[1][j1]*A[0][j2]  + A[2][j1]*A[1][j2]  +
                                A[0][j1]*A[0][j2]*2+ A[1][j1]*A[1][j2]*2+ A[2][j1]*A[2][j2]*2 ) *d; // divide by 120.0f later
                }
            }

            for(int v = 0; v < numOfVerts; v++){
                verts[v].pos -= centerOfMass;
            }
            diag /= volume*(60.0f/6.0f);
            offd /= volume*(120.0f/6.0f);

            return m3(diag.y+diag.z  , -offd.z      , -offd.y,
                    -offd.z        , diag.x+diag.z, -offd.x,
                    -offd.y        , -offd.x      , diag.x+diag.y );
        }

        static float signedTetrahedronVolume(v3 a, v3 b, v3 c){
            return glm::dot(a, glm::cross(b,c))/6.0f;
        }


    };

}