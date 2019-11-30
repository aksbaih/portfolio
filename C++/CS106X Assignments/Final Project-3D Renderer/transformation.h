#ifndef TRANSFORMATION_H
#define TRANSFORMATION_H
#include "matrix.h"
#include "vec.h"
#include <cmath>

class transformation
{
public:
    transformation();

    // Utility functions that generate transformation 3x3 matricies as desired and parametarized
    matrix<3> xRotationMatrix3D(float angle);
    matrix<3> yRotationMatrix3D(float angle);
    matrix<3> zRotationMatrix3D(float angle);
    matrix<3> zoomMatrix3D(float factor);

};

#endif // TRANSFORMATION_H
