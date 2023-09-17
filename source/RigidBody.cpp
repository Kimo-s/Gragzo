#include <RigidBody.hpp>
#include <gragzo.hpp>



RigidBody::RigidBody(const char* filename, Shader &shader) : PhysicalObject(filename, shader){

    initRigidBody();
}


void printVec(glm::vec3 vec){
    printf("(%f,%f,%f)\n", vec.x, vec.y, vec.z);
}


void RigidBody::initRigidBody() {
    float m = 1.0f;

    v3 centerOfMass = glm::vec3(0,0,0);

    Ibody = m3(1.0f);
    mass = 0.0f;

    for(int i = 0; i < verts.size(); i++){
        v3 ri = verts[i].pos;
        mass += m;
        centerOfMass += m*ri;
    }
    centerOfMass /= mass;

    x = v3(0.0);
    q = glm::angleAxis(0.0f, v3(0,0,1.0));
    L = v3(30.0,0.0,0.0);
    v = v3(0.0,0,0);
    omega = v3(0.0,0.0,0.0);

    F = v3(0.0, 0.0, 0.0);

    for(int i = 0; i < verts.size(); i++){
        verts[i].pos = verts[i].pos - centerOfMass;
        v3 ri = verts[i].pos;
        Ibody += m * (glm::dot(ri,ri) * m3(1.0f) - glm::outerProduct(ri, ri));
    }
    Ibodyinv = glm::inverse(Ibody);
    Iinv = Ibodyinv;


    updateBuffers();
}

void RigidBody::simulateTimeStep(float dt){


    v += (F/mass)*dt;
    x += v*dt;
    
    q = glm::normalize(q);
    m3 R = glm::mat3_cast(q);
    Iinv = R*Ibodyinv*glm::transpose(R);

    if(glm::length(L) >= 1.0e-4f){

        omega = Iinv * L;

        glm::quat wq = glm::quat(0, omega.x, omega.y, omega.z);
        q += 0.5f*wq*q*dt;
        q = glm::normalize(q);
    }



    // std::cout << x.x <<  std::endl;
    // printf("%s\n", printVec(omega));
    // printVec(omega);

    modelMat = glm::mat4_cast(q);
    modelMat[3][0] = x.x;
    modelMat[3][1] = x.y;
    modelMat[3][2] = x.z;
    modelMat[3][3] = 1.0f;
}