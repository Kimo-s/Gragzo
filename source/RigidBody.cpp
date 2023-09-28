#include <RigidBody.hpp>
#include <gragzo.hpp>

using namespace gra;


RigidBody::RigidBody(const char* filename, Shader &shader) : PhysicalObject(filename, shader){

    initRigidBody();
}


void printVec(glm::vec3 vec){
    printf("(%f,%f,%f)\n", vec.x, vec.y, vec.z);
}


void RigidBody::initRigidBody() {

    mass = 1.0f*calculateVolume();

    v3 centerOfMass = calculateCenterOfMass();
    this->center = centerOfMass;

    // std::cout << mass << "\n";

    x = v3(0.0);
    q = glm::angleAxis(0.0f, v3(0,0,1.0));
    L = v3(0.0,0.0,0.0);
    v = v3(0.0,0,0.0);
    omega = v3(0.0,0.0,0.0);

    F = v3(0.0, 0.0, 0.0);

    Ibody = claculateInertia(centerOfMass);
    Ibodyinv = glm::inverse(Ibody);
    Iinv = Ibodyinv;


    // LArrow = std::make_shared<Mesh>("../cylinder.obj", *shaderProgram);
    // addDebugMeshs(LArrow);

    updateBuffers();
}

void RigidBody::simulateTimeStep(float dt){

    v += (F/mass)*dt;
    x += v*dt;

    if(glm::length(L) >= 1.0e-4f){
        glm::quat wq = glm::quat(0, omega.x, omega.y, omega.z);
        q += 0.5f*wq*q*dt;
        q = glm::normalize(q);

        m3 R = glm::mat3_cast(q);
        Iinv = R*Ibodyinv*glm::transpose(R);

        omega = Iinv*L;
    }



    // std::cout << LArrow.get()->modelMat[0][0] <<  std::endl;
    // // printf("%s\n", printVec(omega));
    // printVec(omega);



    // LArrow.get()->matchModelMatToVector(v3(0.0, 1.0, 0.0), omega, x, 1.0);
    this->center = x;

    modelMat = glm::mat4_cast(q);
    modelMat[3][0] = x.x;
    modelMat[3][1] = x.y;
    modelMat[3][2] = x.z;
    modelMat[3][3] = 1.0f;
}