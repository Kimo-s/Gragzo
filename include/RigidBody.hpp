#pragma once
#include "mesh.hpp"
#include <glm/gtc/quaternion.hpp>

class RigidBody : public PhysicalObject {
    public:
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


    RigidBody(const char* filename, Shader &shader);

    void initRigidBody();

    void simulateTimeStep(float dt) override;

};