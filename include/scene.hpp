#pragma once
#include <gragzo.hpp>




class Scene {
    public:

    std::vector<std::shared_ptr<PhysicalObject> > meshes;

    Scene(){

    }

    void drawScene(){
        for(const auto& it: meshes){
            it.get()->draw();
        }
    }

    void simulatePhysics(float dt){
        for(const auto& it: meshes){
            it.get()->simulateTimeStep(dt);
        }
    }

    void addMesh(std::shared_ptr<PhysicalObject> mesh){
        meshes.push_back(mesh);
    }


};