#ifndef MATRIX_H
#define MATRIX_H
#include "vec.h"
#include <vector>
#include "error.h"

template <int DIM>
class matrix
{
public:
    matrix();
    matrix<DIM> operator*(const matrix<DIM>& rightMatrix);    // Matrix-matrix miltiplication
    void setEntry(int row, int col, float value);       // Set entry to a value
    vec<DIM>& getCol(int index);                        // returns a column (vec) by reference at specified index
    void addCol( vec<DIM>& newCol);                     // appends a new col (vec) at the end of the matrix
    matrix<DIM>& operator=(matrix<DIM> rightSideMatrix);// deep copy of the matrix on the right
    matrix<DIM>& operator+=(vec<DIM>);                  // adds corresponding entries of provided vec to all cols (vecs) of the matrix
    int getNumCols() const;                             // returns the number of cols in this matrix
    vec<DIM> getCol(int index) const;                   // returns a copy of the specifid col as a vec
    void empty();                                       // clears all the cols from this matrix
private:
    std::vector<vec<DIM> > cols;    // stores all the cols (vecs) of this matrix
};


template <int DIM>
matrix<DIM>::matrix() {
    cols.clear();
}

template <int DIM>
void matrix<DIM>::empty() {
    cols.clear();
}

template <int DIM>
matrix<DIM> matrix<DIM>::operator*(const matrix<DIM>& rightMatrix) {
    matrix<DIM> result;
    for(int currCol = 0; currCol < rightMatrix.getNumCols(); currCol++) {
        vec<DIM> newVec;
        for(int currRow = 0; currRow < DIM; currRow++) {
            newVec.setEntry(currRow, rightMatrix.cols[currCol] * this->cols[currRow]);
        }
        result.addCol(newVec);
    }
    return result;
}

template <int DIM>
void matrix<DIM>::setEntry(int row, int col, float value) {
    if(col >= cols.size()) error("Column index is out of bound.");
    if(row >= DIM) error ("Row is out of bound.");
    cols.at(col)[row] = value;
}

template <int DIM>
vec<DIM>& matrix<DIM>::getCol(int index) {
    if(index >= cols.size()) error ("Column index is out of bound.");
    return cols[index];
}

template <int DIM>
void matrix<DIM>::addCol( vec<DIM>& newCol) {
    cols.push_back(newCol);
}

template <int DIM>
matrix<DIM>& matrix<DIM>::operator=( matrix<DIM> rightSideMatrix) {
    (this->cols).clear();
    for(int i = 0; i < rightSideMatrix.cols.size(); i++) {
        this->cols.push_back(rightSideMatrix.cols[i]);
    }
    return *this;
}

template <int DIM>
int matrix<DIM>::getNumCols() const {
    return cols.size();
}

template <int DIM>
matrix<DIM>& matrix<DIM>::operator+=(vec<DIM> v) {
    for(int i = 0; i < cols.size(); i++) {
        for(int a = 0; a < DIM; a++) {
            cols[i].setEntry(a, cols[i][a] + v[a]);
        }
    }
    return *this;
}

template <int DIM>
vec<DIM> matrix<DIM>::getCol(int index) const {
    return cols[index];
}

template <int DIM>
ostream& operator<<(ostream& os, const matrix<DIM>& m) {
    for(int i = 0; i < m.getNumCols(); i++) os << m.getCol(i) << endl;
    return os;
}

#endif // MATRIX_H
