#pragma once
#include <vector>
#include <glad/glad.h>
#include <unordered_map>
#include <utility>
#include <bits/stdc++.h>
#include <glm/vec3.hpp>
#include <shader.hpp>

#define EPA_TOLERANCE 0.0001
#define EPA_MAX_NUM_FACES 64
#define EPA_MAX_NUM_LOOSE_EDGES 32
#define EPA_MAX_NUM_ITERATIONS 64
#define GJK_MAX_NUM_ITERATIONS 64

// Insipred from https://github.com/kevinmoran/GJK/blob/master/GJK.h

namespace gra {
    using v3 = glm::vec3;


    enum gjkResult {
        NO_INTERSECTION, 
        FOUND_INTERSECTION,
        STILL_EVOLVING
    };

    class gjk {
        public:
        
        std::shared_ptr<Mesh> a, b;
        v3 direction;

        gjk(std::shared_ptr<Mesh> a, std::shared_ptr<Mesh> b): a(a), b(b){
        }

        gjk() {
        }

        void setShapes(std::shared_ptr<Mesh> a, std::shared_ptr<Mesh> b){
            this->a = a;
            this->b = b;
        }

        bool testCollision(v3& pentrationVector){
            return gjkTest(pentrationVector);
        }

        v3 support(v3 dir){
            return a->support(dir) - b->support(-1.0f*dir);
        }


        bool gjkTest(v3& mtv){
            v3 a, b, c, d; //Simplex: just a set of points (a is always most recently added)
            v3 search_dir = this->b->center - this->a->center; //initial search direction between colliders
            // std::cout << glm::length(search_dir) << std::endl;

            //Get initial point for simplex
            c = support(search_dir);
            search_dir = -c; //search in direction of origin

            //Get second point for a line segment simplex
            b = support(search_dir);

            if(dot(b, search_dir)<0) { return false; }//we didn't reach the origin, won't enclose it

            search_dir = cross(cross(c-b,-b),c-b); //search perpendicular to line segment towards origin
            if(search_dir==v3(0,0,0)){ //origin is on this line segment
                //Apparently any normal search vector will do?
                search_dir = cross(c-b, v3(1,0,0)); //normal with x-axis
                if(search_dir==v3(0,0,0)) search_dir = cross(c-b, v3(0,0,-1)); //normal with z-axis
            }
            int simp_dim = 2; //simplex dimension
            
            for(int iterations=0; iterations<GJK_MAX_NUM_ITERATIONS; iterations++)
            {
                a = support(search_dir);
                if(dot(a, search_dir)<0) { return false; }//we didn't reach the origin, won't enclose it
            
                simp_dim++;
                if(simp_dim==3){
                    update_simplex3(a,b,c,d,simp_dim,search_dir);
                }
                else if(update_simplex4(a,b,c,d,simp_dim,search_dir)) {
                    if(&mtv) mtv = EPA(a,b,c,d);
                    return true;
                }
            }//endfor
            return false;
        }

        //Triangle case
        void update_simplex3(v3 &a, v3 &b, v3 &c, v3 &d, int &simp_dim, v3 &search_dir){
            /* Required winding order:
            //  b
            //  | \
            //  |   \
            //  |    a
            //  |   /
            //  | /
            //  c
            */
            v3 n = glm::cross(b-a, c-a); //triangle's normal
            v3 AO = -a; //direction to origin

            //Determine which feature is closest to origin, make that the new simplex

            simp_dim = 2;
            if(dot(cross(b-a, n), AO)>0){ //Closest to edge AB
                c = a;
                //simp_dim = 2;
                search_dir = cross(cross(b-a, AO), b-a);
                return;
            }
            if(dot(cross(n, c-a), AO)>0){ //Closest to edge AC
                b = a;
                //simp_dim = 2;
                search_dir = cross(cross(c-a, AO), c-a);
                return;
            }
            
            simp_dim = 3;
            if(dot(n, AO)>0){ //Above triangle
                d = c;
                c = b;
                b = a;
                //simp_dim = 3;
                search_dir = n;
                return;
            }
            //else //Below triangle
            d = b;
            b = a;
            //simp_dim = 3;
            search_dir = -n;
            return;
        }

        //Tetrahedral case
        bool update_simplex4(v3 &a, v3 &b, v3 &c, v3 &d, int &simp_dim, v3 &search_dir){
            // a is peak/tip of pyramid, BCD is the base (counterclockwise winding order)
            //We know a priori that origin is above BCD and below a

            //Get normals of three new faces
            v3 ABC = cross(b-a, c-a);
            v3 ACD = cross(c-a, d-a);
            v3 ADB = cross(d-a, b-a);

            v3 AO = -a; //dir to origin
            simp_dim = 3; //hoisting this just cause

            //Plane-test origin with 3 faces
            /*
            // Note: Kind of primitive approach used here; If origin is in front of a face, just use it as the new simplex.
            // We just go through the faces sequentially and exit at the first one which satisfies dot product. Not sure this 
            // is optimal or if edges should be considered as possible simplices? Thinking this through in my head I feel like 
            // this method is good enough. Makes no difference for AABBS, should test with more complex colliders.
            */
            if(dot(ABC, AO)>0){ //In front of ABC
                d = c;
                c = b;
                b = a;
                search_dir = ABC;
                return false;
            }
            if(dot(ACD, AO)>0){ //In front of ACD
                b = a;
                search_dir = ACD;
                return false;
            }
            if(dot(ADB, AO)>0){ //In front of ADB
                c = d;
                d = b;
                b = a;
                search_dir = ADB;
                return false;
            }

            //else inside tetrahedron; enclosed!
            return true;

            //Note: in the case where two of the faces have similar normals,
            //The origin could conceivably be closest to an edge on the tetrahedron
            //Right now I don't think it'll make a difference to limit our new simplices
            //to just one of the faces, maybe test it later.
        }


        v3 EPA(v3 a, v3 b, v3 c, v3 d){
            v3 faces[EPA_MAX_NUM_FACES][4];

            //Init with final simplex from GJK
            faces[0][0] = a;
            faces[0][1] = b;
            faces[0][2] = c;
            faces[0][3] = glm::normalize(glm::cross(b-a, c-a)); //ABC
            faces[1][0] = a;
            faces[1][1] = c;
            faces[1][2] = d;
            faces[1][3] = glm::normalize(glm::cross(c-a, d-a)); //ACD
            faces[2][0] = a;
            faces[2][1] = d;
            faces[2][2] = b;
            faces[2][3] = glm::normalize(glm::cross(d-a, b-a)); //ADB
            faces[3][0] = b;
            faces[3][1] = d;
            faces[3][2] = c;
            faces[3][3] = glm::normalize(glm::cross(d-b, c-b)); //BDC


            int num_faces=4;
            int closest_face;

            for(int iterations=0; iterations<EPA_MAX_NUM_ITERATIONS; iterations++){
                //Find face that's closest to origin
                float min_dist = dot(faces[0][0], faces[0][3]);
                closest_face = 0;
                for(int i=1; i<num_faces; i++){
                    float dist = dot(faces[i][0], faces[i][3]);
                    if(dist<min_dist){
                        min_dist = dist;
                        closest_face = i;
                    }
                }
                        //search normal to face that's closest to origin
                v3 search_dir = faces[closest_face][3]; 
                v3 p = this->a->support(search_dir) - this->b->support(-search_dir);

                if(dot(p, search_dir)-min_dist<EPA_TOLERANCE){
                    //Convergence (new point is not significantly further from origin)
                    return faces[closest_face][3]*dot(p, search_dir); //dot vertex with normal to resolve collision along normal!
                }

                v3 loose_edges[EPA_MAX_NUM_LOOSE_EDGES][2]; //keep track of edges we need to fix after removing faces
                int num_loose_edges = 0;

                //Find all triangles that are facing p
                for(int i=0; i<num_faces; i++)
                {
                    if(dot(faces[i][3], p-faces[i][0] )>0) //triangle i faces p, remove it
                    {
                        //Add removed triangle's edges to loose edge list.
                        //If it's already there, remove it (both triangles it belonged to are gone)
                        for(int j=0; j<3; j++) //Three edges per face
                        {
                            v3 current_edge[2] = {faces[i][j], faces[i][(j+1)%3]};
                            bool found_edge = false;
                            for(int k=0; k<num_loose_edges; k++) //Check if current edge is already in list
                            {
                                if(loose_edges[k][1]==current_edge[0] && loose_edges[k][0]==current_edge[1]){
                                    //Edge is already in the list, remove it
                                    //THIS ASSUMES EDGE CAN ONLY BE SHARED BY 2 TRIANGLES (which should be true)
                                    //THIS ALSO ASSUMES SHARED EDGE WILL BE REVERSED IN THE TRIANGLES (which 
                                    //should be true provided every triangle is wound CCW)
                                    loose_edges[k][0] = loose_edges[num_loose_edges-1][0]; //Overwrite current edge
                                    loose_edges[k][1] = loose_edges[num_loose_edges-1][1]; //with last edge in list
                                    num_loose_edges--;
                                    found_edge = true;
                                    k=num_loose_edges; //exit loop because edge can only be shared once
                                }
                            }//endfor loose_edges

                            if(!found_edge){ //add current edge to list
                                // assert(num_loose_edges<EPA_MAX_NUM_LOOSE_EDGES);
                                if(num_loose_edges>=EPA_MAX_NUM_LOOSE_EDGES) break;
                                loose_edges[num_loose_edges][0] = current_edge[0];
                                loose_edges[num_loose_edges][1] = current_edge[1];
                                num_loose_edges++;
                            }
                        }

                        //Remove triangle i from list
                        faces[i][0] = faces[num_faces-1][0];
                        faces[i][1] = faces[num_faces-1][1];
                        faces[i][2] = faces[num_faces-1][2];
                        faces[i][3] = faces[num_faces-1][3];
                        num_faces--;
                        i--;
                    }//endif p can see triangle i
                }//endfor num_faces
                
                //Reconstruct polytope with p added
                for(int i=0; i<num_loose_edges; i++)
                {
                    // assert(num_faces<EPA_MAX_NUM_FACES);
                    if(num_faces>=EPA_MAX_NUM_FACES) break;
                    faces[num_faces][0] = loose_edges[i][0];
                    faces[num_faces][1] = loose_edges[i][1];
                    faces[num_faces][2] = p;
                    faces[num_faces][3] = glm::normalize(glm::cross(loose_edges[i][0]-loose_edges[i][1], loose_edges[i][0]-p));

                    //Check for wrong normal to maintain CCW winding
                    float bias = 0.000001; //in case dot result is only slightly < 0 (because origin is on face)
                    if(dot(faces[num_faces][0], faces[num_faces][3])+bias < 0){
                        v3 temp = faces[num_faces][0];
                        faces[num_faces][0] = faces[num_faces][1];
                        faces[num_faces][1] = temp;
                        faces[num_faces][3] = -faces[num_faces][3];
                    }
                    num_faces++;
                }
            } //End for iterations
            printf("EPA did not converge\n");
            //Return most recent closest point
            return faces[closest_face][3] * dot(faces[closest_face][0], faces[closest_face][3]);
        }

    };
    
}