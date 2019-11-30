#include "transformation.h"

transformation::transformation()
{

}
matrix<3> transformation::xRotationMatrix3D(float angle) {
    matrix<3> M;
    vec<3> row1(1, 0, 0);
    M.addCol(row1);
    vec<3> row2(0, cos(angle), -sin(angle));
    M.addCol(row2);
    vec<3> row3(0, sin(angle), cos(angle));
    M.addCol(row3);
    return M;
}
matrix<3> transformation::yRotationMatrix3D(float angle) {
    matrix<3> M;
    vec<3> row2(cos(angle), 0, sin(angle));
    M.addCol(row2);
    vec<3> row1(0, 1, 0);
    M.addCol(row1);
    vec<3> row3(-sin(angle), 0, cos(angle));
    M.addCol(row3);
    return M;
}
matrix<3> transformation::zRotationMatrix3D(float angle) {
    matrix<3> M;
    vec<3> row2(cos(angle), -sin(angle), 0);
    M.addCol(row2);
    vec<3> row3(sin(angle), cos(angle), 0);
    M.addCol(row3);
    vec<3> row1(0, 0, 1);
    M.addCol(row1);
    return M;
}
matrix<3> transformation::zoomMatrix3D(float factor) {
    matrix<3> result;
    for(int i = 0; i < 3; i ++) {
        vec<3> v;
        for(int a = 0; a < 3; a++) {
            v.setEntry(a, (a == i) ? factor : 0);
        }
        result.addCol(v);
    }
    return result;
}
