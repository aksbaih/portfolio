#include "triangle.h"
#include "vec.h"

triangle::triangle(int vertix1, int vertix2, int vertix3, int normal) {
    verticies[0] = vertix1;
    verticies[1] = vertix2;
    verticies[2] = vertix3;
    this->normal = normal;
}

vec<3> triangle::getNormal() const {
    normsL->getCol(normal).normalize();
    return normsL->getCol(normal);
}

float triangle::findBrightness(vec<3>& kLightVec) { // find the cosine
    return kLightVec * getNormal();
}

float triangle::getYComp() const {
    return vertsL->getCol(verticies[0])[1];
}

float triangle::e(int v, int c) const {
    return vertsL->getCol(verticies[v])[c];
}

vec<3> triangle::findNormal() {

    vec<3> normal;
    vec<3> a, b;

    // The following algorithm is a cross product

    for(int i = 0; i < 3; i ++) {
        a.setEntry(i, e(1, i) - e(0, i));
        b.setEntry(i, e(2, i) - e(0, i));
    }

    normal.setEntry(0, a[1] * b[2] - a[2] * b[1]);
    normal.setEntry(1, a[2] * b[0] - a[0] * b[2]);
    normal.setEntry(2, a[0] * b[1] - a[1] * b[0]);

    normal.normalize();
    return normal;
}
