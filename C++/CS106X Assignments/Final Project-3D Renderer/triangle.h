#ifndef TRIANGLE_H
#define TRIANGLE_H
#include "vec.h"
#include "matrix.h"
#include "vector.h"


class triangle
{
public:
    // initializes with the indicies provided
    triangle(int vertix1, int vertix2, int vertix3, int normal);

    // returns the 3-vector representing the normal of the current triangle
    vec<3> getNormal() const;

    // returns a float representing the cosine of the angle between the normal and light vec
    float findBrightness(vec<3>& kLightVec);

    // returns the y component of the first vertix (used for drawing ordering from back to front)
    float getYComp() const;

    // returns the value of 'comp' component of the 'vertix' specified
    float e(int vertix, int comp) const; // returns the specific entry of a vertix

    // calculates the normal vector for the triangle (not used in this app but can be used for future updates)
    vec<3> findNormal();

    // pointers to the matricies that include the verticies and normals respectively
    matrix<3>* vertsL;
    matrix<3>* normsL;

private:

    int verticies [3];  // the indicies of the verticies
    int normal;         // the index of the normal
};

/* function: loadTriagnles
 * This function takes a matrix 'faces' that contains the configurations of all faces
 * and translates it into new allocated 'triangles' saving their pointers to vector 'triangles'
 */
static void loadTriagnles(Vector<triangle *>& triangles, matrix<3> &Verts, matrix<3> &Norms, matrix<4>& faces) {
    for(int i = 0; i < faces.getNumCols(); i++) {

        triangle * tri = new triangle(
                    faces.getCol(i)[0],
                    faces.getCol(i)[1],
                    faces.getCol(i)[2],
                    faces.getCol(i)[3]
                );

        tri->normsL = &Norms;   // pointers to the verticies and normals
        tri->vertsL = &Verts;

        triangles.add(tri);
    }
}

#endif // TRIANGLE_H
