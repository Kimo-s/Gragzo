#pragma once
// #include <mesh.hpp>
#include <vector>
#include <glad/glad.h>
#include <unordered_map>
#include <utility>
#include <bits/stdc++.h>
#include <glm/vec3.hpp>
#include <shader.hpp>



namespace gra {

    // enum ShapeType
    // {
    //     General, Point, Trian, Plane, Box, Pyramid, Cylinder, Sphere, Lane
    // };

    class Shape {
        public:

        std::vector<Vertex> verts;

        Shape(std::vector<Vertex>& verts){
            this->verts = verts;
        }

        glm::vec3 support(glm::vec3 direction){
            glm::vec3 v = verts[0].pos;
            float maxDot = glm::dot(v, direction);
            for(int i = 0; i < verts.size(); i++){
                float d = glm::dot(verts[i].pos, direction);
                if(d >= maxDot){
                    v = verts[i].pos;
                    maxDot = d;
                }
            }
            return v;
        }

    };


}