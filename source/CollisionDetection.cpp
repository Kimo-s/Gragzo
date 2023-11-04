#include <CollisionDetection.hpp>
#include <gjk.hpp>
#include <gui_parameters.hpp>

using namespace gra;


extern gui_parameters guiParameters;

void gra::CollisionDetector::detectCollisions()
{
    v3 penVec(0.0f);

    for(int i = 0; i < meshes.size(); i++){
        for(int q = i; q < meshes.size(); q++){
            if(i != q){
                CollisionInfo info;
                bool res = GJKCalculation(meshes[q].get(), meshes[i].get(), info);
                // std::cout << "Something\n";
                if(res){
                    // setColor(meshes[q], v3(1.0, 1.0, 0.0));
                    // setColor(meshes[i], v3(1.0, 1.0, 0.0));
                    // std::cout << glm::length(penVec) << std::endl;
                    // meshes[q]->x += penVec;
                    // meshes[q]->v = penVec;
                    // meshes[i]->x = penVec;
                    updateMeshMotion(meshes[q], meshes[i], info);
                }
            }

        }
    }

}

v3 pt_velocity(RigidBody::Ptr body, v3 p){
    return body->v + glm::cross(body->omega, p-body->x);
}

void gra::CollisionDetector::updateMeshMotion(RigidBody::Ptr a, RigidBody::Ptr b, CollisionInfo& info){
    float dt = 0.016f;
    float epsilon = guiParameters.restitution;

    if(!a->isStatic){
        a->x -= info.normal * info.penetration;
    }

    if(!b->isStatic){
        b->x += info.normal * info.penetration;
    }

    v3 p = info.contactA; // + info.normal * info.penetration;

    v3 padot = pt_velocity(a, p);
    v3 pbdot = pt_velocity(b, p);
    v3 n = info.normal;
    v3 ra = p - a->x;
    v3 rb = p - b->x;
    
    float vrel = glm::dot(n, (padot - pbdot));
    float numerator = -(1.0f + epsilon) * vrel;

    float term1 = 1 / a->mass;
    float term2 = 1 / b->mass;
    float term3 = glm::dot(n, glm::cross(a->Iinv * glm::cross(ra, n), ra));
    float term4 = glm::dot(n, glm::cross(b->Iinv * glm::cross(rb, n), rb));

    float j;

    glm::vec3 impluse;

    if(b->isStatic) {
        j = numerator / (term1 + term3);
    } else if(a->isStatic) {
        j = numerator / (term2 + term4);
    } else {
        j = numerator / (term1 + term2 + term3 + term4);
    }

    impluse = j*n;

    
    if(!a->isStatic){
        a->v += (impluse/a->mass) - (a->v*0.3f/a->mass);
        a->L += glm::cross(ra,impluse) - a->L*0.03f;
        a->omega = a->Iinv * a->L;
    }   

    if(!b->isStatic){
        b->v += -(impluse/b->mass) - (b->v*0.3f/b->mass);
        b->L += -glm::cross(rb,impluse) - b->L*0.03f;
        b->omega = b->Iinv * b->L;
    }

}