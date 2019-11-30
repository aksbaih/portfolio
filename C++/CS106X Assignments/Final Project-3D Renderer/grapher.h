#ifndef GRAPHER_H
#define GRAPHER_H

#include <gwindow.h>
#include <gobjects.h>
#include "vec.h"
#include "matrix.h"
#include "pqueue.h"
#include "set.h"
#include "vector.h"
#include "transformation.h"
#include "triangle.h"
#include "gslider.h"

class grapher
{
public:
    grapher();
    ~grapher();

    /*  helper function: drawTriangle
     *  This function draws a single triangle pointed to by 'tri' with a shade of gray signaled by the cosine of the
     *  angle between the normal of the triangle and the light vector. It also translates all the points by the translation vector provided
     */
    void drawTriangle(triangle * tri, float cosineLight, vec<3>& translation);

    /* function: visualize3Dmatrix
     * This function draws all the triangles in the vector 'tris' with respect to a specific light vector and translation vector
     */
    void visualize3Dmatrix(Vector<triangle *>& tris, vec<3>& LightVec, vec<3> translation);

    const float kWindowWidth = 900;
    const float kWindowHeight = 900;

    void close();   // closes the window after deleteing any dynamically allocated memory

private:

    const float kMinBrightness = 10;
    const float kBrightnessRange = 240;
    GWindow window; // the window showing the model
};

#endif // GRAPHER_H
