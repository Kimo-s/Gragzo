#pragma once
#include <glad/glad.h>
#include <glm/vec3.hpp>
#include <shader.hpp>
#include "mesh.hpp"
#include "CollisionDetection.hpp"

#define EPA_TOLERANCE 0.0001
#define EPA_MAX_NUM_FACES 64
#define EPA_MAX_NUM_LOOSE_EDGES 32
#define EPA_MAX_NUM_ITERATIONS 64
#define GJK_MAX_NUM_ITERATIONS 64

// from https://github.com/kevinmoran/GJK/blob/master/GJK.h

namespace gra {
    using v3 = glm::vec3;

    
    struct Point {
		glm::vec3 p; //Conserve Minkowski Difference
		glm::vec3 a; //Result coordinate of object A's support function 
		glm::vec3 b; //Result coordinate of object B's support function 
	};




    //https://github.com/Another-Ghost/3D-Collision-Detection-and-Resolution-Using-GJK-and-EPA/blob/master/CSC8503/CSC8503Common/GJK.cpp#L322
    //Gilbert¨CJohnson¨CKeerthi distance algorithm
	bool GJKCalculation(Mesh* coll1, Mesh* coll2, CollisionInfo& collisionInfo); 

	//Internal functions used in the GJK algorithm
	void update_simplex3(Point& a, Point& b, Point& c, Point& d, int& simp_dim, v3& search_dir);
	bool update_simplex4(Point& a, Point& b, Point& c, Point& d, int& simp_dim, v3& search_dir);

	//Expanding Polytope Algorithm. 
	void EPA(Point& a, Point& b, Point& c, Point& d, Mesh* coll1, Mesh* coll2, CollisionInfo& collisionInfo);


    void CalculateSearchPoint(Point& point, glm::vec3& search_dir, Mesh* coll1, Mesh* coll2);

    
}