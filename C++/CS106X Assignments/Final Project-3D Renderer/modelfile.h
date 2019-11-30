#ifndef MODELFILE_H
#define MODELFILE_H
#include <fstream>
#include "matrix.h"
#include <string>
#include <sstream>
#include "strlib.h"
#include <iostream>
#include "triangle.h"

class modelfile
{
public:
    modelfile();

    /* function: loadObject
     * This function loads a '.obj' file specified by 'fileName' and parses it into:
     * 1. A matrix of verticies (verts) each col of which is a point.
     * 2. A matrix of normals (norms) each col of which is a normal vector for some face
     * 3. A matric of configurations (faces) wach col of which consists of three indicies
     *      indicating the verticies of this face and an index for its normal vector.
     * It does all of this according to the conventional '.obj' file specifications
     */
    void loadObject(std::string fileName, matrix<3>& verts, matrix<3>& norms, matrix<4>& faces);
};

#endif // MODELFILE_H
