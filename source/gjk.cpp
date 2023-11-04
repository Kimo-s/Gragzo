#include "gjk.hpp"


// from https://github.com/kevinmoran/GJK/blob/master/GJK.h


#define GJK_MAX_NUM_ITERATIONS 64


void Barycentric(const gra::v3& a, const gra::v3& b, const gra::v3& c, const gra::v3& p, float& u, float& v, float& w)
{
	gra::v3 v0 = b - a, v1 = c - a, v2 = p - a;
	float d00 = glm::dot(v0, v0);
	float d01 = glm::dot(v0, v1);
	float d11 = glm::dot(v1, v1);
	float d20 = glm::dot(v2, v0);
	float d21 = glm::dot(v2, v1);
	float denom = d00 * d11 - d01 * d01;
	v = (d11 * d20 - d01 * d21) / denom;
	w = (d00 * d21 - d01 * d20) / denom;
	u = 1.0f - v - w;

}


bool gra::GJKCalculation(Mesh* coll1, Mesh* coll2, CollisionInfo& collisionInfo)
{

	v3* mtv;

	v3 coll1Pos = coll1->center;
	v3 coll2Pos = coll2->center;


	Point a, b, c, d; //Simplex: just a set of points (a is always most recently added)
	v3 search_dir = coll1Pos - coll2Pos; //initial search direction between colliders

	 //Get initial point for simplex
	//Point c;
	CalculateSearchPoint(c, search_dir, coll1, coll2);
	search_dir = -c.p; //search in direction of origin

	//Get second point for a line segment simplex
	//Point b;
	CalculateSearchPoint(b, search_dir, coll1, coll2);

	if (glm::dot(b.p, search_dir) < 0) {
		return false;
	}//we didn't reach the origin, won't enclose it

	search_dir = glm::cross(glm::cross(c.p - b.p, -b.p), c.p - b.p); //search perpendicular to line segment towards origin
	if (search_dir == v3(0, 0, 0)) { //origin is on this line segment
		//Apparently any normal search vector will do?
		search_dir = glm::cross(c.p - b.p, v3(1, 0, 0)); //normal with x-axis
		if (search_dir == v3(0, 0, 0))
			search_dir = glm::cross(c.p - b.p, v3(0, 0, -1)); //normal with z-axis
	}
	int simp_dim = 2; //simplex dimension

	for (int iterations = 0; iterations < GJK_MAX_NUM_ITERATIONS; iterations++)
	{
		//Point a;
		CalculateSearchPoint(a, search_dir, coll1, coll2);

		if (glm::dot(a.p, search_dir) < 0) {
			return false;
		}//we didn't reach the origin, won't enclose it

		simp_dim++;
		if (simp_dim == 3) {
			update_simplex3(a, b, c, d, simp_dim, search_dir);
		}
		else if (update_simplex4(a, b, c, d, simp_dim, search_dir)) {
			EPA(a, b, c, d, coll1, coll2, collisionInfo);
			return true;
		}
	}//endfor

	return false;
}

void gra::update_simplex3(Point& a, Point& b, Point& c, Point& d, int& simp_dim, v3& search_dir)
{
	/* Required winding order:
	   //  b
	   //  | \
	   //  |   \
	   //  |    a
	   //  |   /
	   //  | /
	   //  c
	   */
	v3 n = glm::cross(b.p - a.p, c.p - a.p); //triangle's normal
	v3 AO = -a.p; //direction to origin

	//Determine which feature is closest to origin, make that the new simplex

	simp_dim = 2;
	if (glm::dot(glm::cross(b.p - a.p, n), AO) > 0) { //Closest to edge AB
		c = a;
		//simp_dim = 2;
		search_dir = glm::cross(glm::cross(b.p - a.p, AO), b.p - a.p);
		return;
	}
	if (glm::dot(glm::cross(n, c.p - a.p), AO) > 0) { //Closest to edge AC
		b = a;
		//simp_dim = 2;
		search_dir = glm::cross(glm::cross(c.p - a.p, AO), c.p - a.p);
		return;
	}

	simp_dim = 3;
	if (glm::dot(n, AO) > 0) { //Above triangle
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

bool gra::update_simplex4(Point& a, Point& b, Point& c, Point& d, int& simp_dim, v3& search_dir)
{
	// a is peak/tip of pyramid, BCD is the base (counterclockwise winding order)
	//We know a priori that origin is above BCD and below a

	//Get normals of three new faces
	v3 ABC = glm::cross(b.p - a.p, c.p - a.p);
	v3 ACD = glm::cross(c.p - a.p, d.p - a.p);
	v3 ADB = glm::cross(d.p - a.p, b.p - a.p);

	v3 AO = -a.p; //dir to origin
	simp_dim = 3; //hoisting this just cause

	//Plane-test origin with 3 faces
	/*
	// Note: Kind of primitive approach used here; If origin is in front of a face, just use it as the new simplex.
	// We just go through the faces sequentially and exit at the first one which satisfies dot product. Not sure this
	// is optimal or if edges should be considered as possible simplices? Thinking this through in my head I feel like
	// this method is good enough. Makes no difference for AABBS, should test with more complex colliders.
	*/
	if (glm::dot(ABC, AO) > 0) { //In front of ABC
		d = c;
		c = b;
		b = a;
		search_dir = ABC;
		return false;
	}

	if (glm::dot(ACD, AO) > 0) { //In front of ACD
		b = a;
		search_dir = ACD;
		return false;
	}
	if (glm::dot(ADB, AO) > 0) { //In front of ADB
		c = d;
		d = b;
		b = a;
		search_dir = ADB;
		return false;
	}

	//else inside tetrahedron; enclosed!
	return true;
}

//Expanding Polytope Algorithm
#define EPA_TOLERANCE 0.0001
#define EPA_MAX_NUM_FACES 64
#define EPA_MAX_NUM_LOOSE_EDGES 32
#define EPA_MAX_NUM_ITERATIONS 64
void gra::EPA(Point& a, Point& b, Point& c, Point& d, Mesh* coll1, Mesh* coll2, CollisionInfo& collisionInfo)
{
	Point faces[EPA_MAX_NUM_FACES][4]; //Array of faces, each with 3 verts and a normal

	v3 VertexA[3];
	v3 VertexB[3];

	//Init with final simplex from GJK
	faces[0][0] = a;
	faces[0][1] = b;
	faces[0][2] = c;
	faces[0][3].p = glm::normalize(glm::cross(b.p - a.p, c.p - a.p)); //ABC
	faces[1][0] = a;
	faces[1][1] = c;
	faces[1][2] = d;
	faces[1][3].p = glm::normalize(glm::cross(c.p - a.p, d.p - a.p)); //ACD
	faces[2][0] = a;
	faces[2][1] = d;
	faces[2][2] = b;
	faces[2][3].p = glm::normalize(glm::cross(d.p - a.p, b.p - a.p)); //ADB
	faces[3][0] = b;
	faces[3][1] = d;
	faces[3][2] = c;
	faces[3][3].p = glm::normalize(glm::cross(d.p - b.p, c.p - b.p)); //BDC

	int num_faces = 4;
	int closest_face;

	for (int iterations = 0; iterations < EPA_MAX_NUM_ITERATIONS; iterations++) {
		//Find face that's closest to origin
		float min_dist = glm::dot(faces[0][0].p, faces[0][3].p);
		closest_face = 0;
		for (int i = 1; i < num_faces; i++) {
			float dist = glm::dot(faces[i][0].p, faces[i][3].p);
			if (dist < min_dist) {
				min_dist = dist;
				closest_face = i;
			}
		}

		//search normal to face that's closest to origin
		v3 search_dir = faces[closest_face][3].p;

		Point p;
		CalculateSearchPoint(p, search_dir, coll1, coll2);

		if (glm::dot(p.p, search_dir) - min_dist < EPA_TOLERANCE) {

		/*Core of calculating collision information*/
			v3 v1v0 = faces[closest_face][1].p - faces[closest_face][0].p;
			v3 v2v0 = faces[closest_face][2].p - faces[closest_face][0].p;

			v3 planeNormal = glm::normalize(glm::cross(v1v0, v2v0));

			float d = -glm::dot(faces[closest_face][0].p, planeNormal);

			v3 projectionPoint = v3(0, 0, 0) - (glm::dot(v3(0, 0, 0), planeNormal)+d) * planeNormal;

			float u, v, w;
			Barycentric(faces[closest_face][0].p, faces[closest_face][1].p, faces[closest_face][2].p,
				projectionPoint, u, v, w); //finding the barycentric coordinate of this projection point to the triangle

			//The contact points just have the same barycentric coordinate in their own triangles which  are composed by result coordinates of support function 
			v3 localA = faces[closest_face][0].a * u + faces[closest_face][1].a * v + faces[closest_face][2].a * w;
			v3 localB = faces[closest_face][0].b * u + faces[closest_face][1].b * v + faces[closest_face][2].b * w;
			float penetration = glm::length(localA - localB);
			v3 normal = glm::normalize(localA - localB);

			//Convergence (new point is not significantly further from origin)
			// localA -= coll1->center;
			// localB -= coll2->center;

			collisionInfo = {};
			collisionInfo.contactA = localA;
			collisionInfo.contactB = localB;
			collisionInfo.normal = normal;
			collisionInfo.penetration = penetration;
		/*Core of calculating collision information*/

			return;
		}

		Point loose_edges[EPA_MAX_NUM_LOOSE_EDGES][2]; //keep track of edges we need to fix after removing faces
		int num_loose_edges = 0;

		//Find all triangles that are facing p
		for (int i = 0; i < num_faces; i++)
		{
			if (glm::dot(faces[i][3].p, p.p - faces[i][0].p) > 0) //triangle i faces p, remove it
			{
				//Add removed triangle's edges to loose edge list.
				//If it's already there, remove it (both triangles it belonged to are gone)
				for (int j = 0; j < 3; j++) //Three edges per face
				{
					Point current_edge[2] = { faces[i][j], faces[i][(j + 1) % 3] };
					bool found_edge = false;
					for (int k = 0; k < num_loose_edges; k++) //Check if current edge is already in list
					{
						if (loose_edges[k][1].p == current_edge[0].p && loose_edges[k][0].p == current_edge[1].p) {
							loose_edges[k][0] = loose_edges[num_loose_edges - 1][0]; //Overwrite current edge
							loose_edges[k][1] = loose_edges[num_loose_edges - 1][1]; //with last edge in list
							num_loose_edges--;
							found_edge = true;
							k = num_loose_edges; //exit loop because edge can only be shared once
						}
					}//endfor loose_edges

					if (!found_edge) { //add current edge to list
						// assert(num_loose_edges<EPA_MAX_NUM_LOOSE_EDGES);
						if (num_loose_edges >= EPA_MAX_NUM_LOOSE_EDGES) break;
						loose_edges[num_loose_edges][0] = current_edge[0];
						loose_edges[num_loose_edges][1] = current_edge[1];
						num_loose_edges++;
					}
				}

				//Remove triangle i from list
				faces[i][0] = faces[num_faces - 1][0];
				faces[i][1] = faces[num_faces - 1][1];
				faces[i][2] = faces[num_faces - 1][2];
				faces[i][3] = faces[num_faces - 1][3];
				num_faces--;
				i--;
			}//endif p can see triangle i
		}//endfor num_faces

		//Reconstruct polytope with p added
		for (int i = 0; i < num_loose_edges; i++)
		{
			// assert(num_faces<EPA_MAX_NUM_FACES);
			if (num_faces >= EPA_MAX_NUM_FACES) break;
			faces[num_faces][0] = loose_edges[i][0];
			faces[num_faces][1] = loose_edges[i][1];
			faces[num_faces][2] = p;
			faces[num_faces][3].p = glm::normalize(glm::cross(loose_edges[i][0].p - loose_edges[i][1].p, loose_edges[i][0].p - p.p));

			//Check for wrong normal to maintain CCW winding
			float bias = 0.000001; //in case dot result is only slightly < 0 (because origin is on face)
			if (glm::dot(faces[num_faces][0].p, faces[num_faces][3].p) + bias < 0) {
				Point temp = faces[num_faces][0];
				faces[num_faces][0] = faces[num_faces][1];
				faces[num_faces][1] = temp;
				faces[num_faces][3].p = -faces[num_faces][3].p;
			}
			num_faces++;
		}
	} //End for iterations
	printf("EPA did not converge\n");
	//Return most recent closest point
	v3 v1v0 = faces[closest_face][1].p - faces[closest_face][0].p;
	v3 v2v0 = faces[closest_face][2].p - faces[closest_face][0].p;

	v3 planeNormal = glm::normalize(glm::cross(v1v0, v2v0));

	float dp = -glm::dot(faces[closest_face][0].p, planeNormal);

	v3 projectionPoint = v3(0, 0, 0) - (dp + glm::dot(glm::vec3(0, 0, 0), planeNormal)) * planeNormal;

	float u, v, w;
	Barycentric(faces[closest_face][0].p, faces[closest_face][1].p, faces[closest_face][2].p,
		projectionPoint, u, v, w); //finding the barycentric coordinate of this projection point to the triangle

	//The contact points just have the same barycentric coordinate in their own triangles which  are composed by result coordinates of support function 
	v3 localA = faces[closest_face][0].a * u + faces[closest_face][1].a * v + faces[closest_face][2].a * w;
	v3 localB = faces[closest_face][0].b * u + faces[closest_face][1].b * v + faces[closest_face][2].b * w;
	float penetration = glm::length(localA - localB);
	v3 normal = glm::normalize(localA - localB);

	//Convergence (new point is not significantly further from origin)
	// localA -= coll1->center;
	// localB -= coll2->center;

	collisionInfo = {};
	collisionInfo.contactA = localA;
	collisionInfo.contactB = localB;
	collisionInfo.normal = normal;
	collisionInfo.penetration = penetration;

	return;
}

void gra::CalculateSearchPoint(Point& point, v3& search_dir, Mesh* coll1, Mesh* coll2)
{
	point.b = coll2->support(search_dir);
	point.a = coll1->support(-search_dir);
	point.p = point.b - point.a;
}