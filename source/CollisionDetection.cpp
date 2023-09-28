#include <gragzo.hpp>
#include <CollisionDetection.hpp>
#include <gjkSimplex.hpp>

using namespace gra;

// void CollisionDetector::detectCollisions(){
//     gjk collisionTester;

//     for(int i = 0; i < meshes.size(); i++){
//         for(int q = i; q < meshes.size(); q++){
//             if(i != q){
//                 collisionTester.setShapes(meshes[i], meshes[q]);
//                 bool res = collisionTester.testCollision();
//                 if(res){
//                     std::cout << "Deteced a collision\n";
//                 }
//                 // checkCollision(meshes[i].get(), meshes[q].get());
//             }

//         }
//     }

// }

// gra::v3 gra::tripleProduct(gra::v3 a, gra::v3 b, gra::v3 c){
//     return glm::cross(glm::cross(a,b),c);
// }