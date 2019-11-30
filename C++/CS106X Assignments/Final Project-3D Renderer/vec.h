#ifndef VEC_H
#define VEC_H
#include <cmath>
#include <iostream>
using namespace std;

template<int DIM>
class vec
{
public:

    // handy initializers for different sizes of vectors
    vec(float, float);
    vec(float, float, float);
    vec();

    float operator*(const vec<DIM>& rightVec) const; // dot product
    float operator[](int) const;    // returns the value of an entry without reference
    vec<DIM>& operator=(vec<DIM>& rightVec);    // deep copy
    vec<DIM> operator-(const vec<DIM>& rightVec);   // substitute corresponding entries
    template<int RDIM> vec<DIM>& operator=( vec<RDIM>& rightVec); // deep copy for vectors with different dimensions (ignore extra entries)
    void setEntry(int i, float v) { entries[i] = v; }   // give a value to an entry
    void normalize();   // change entries to make it a unit vector pointing in the same direction

private:
    float entries [DIM];
};

template <int DIM>
vec<DIM>::vec(float e1, float e2) {
    entries[0] = e1;
    entries[1] = e2;
}

template <int DIM>
vec<DIM>::vec(float e1, float e2, float e3) {
    entries[0] = e1;
    entries[1] = e2;
    entries[2] = e3;
}

template <int DIM>
vec<DIM>::vec() {
    for(int i = 0; i < DIM; i++) entries[i] = 0;
}

template <int DIM>
float vec<DIM>::operator*(const vec<DIM>& rightVec) const {  // dot product
    float product = 0;
    for(int i = 0; i < DIM; i++) product += (this->entries)[i] * rightVec.entries[i];
    return product;
}

template <int DIM>
float vec<DIM>::operator[]( int index) const {
    return entries[index];
}

template<int DIM>
vec<DIM>& vec<DIM>::operator=( vec<DIM>& rightVec){
    for(int i = 0; i < DIM; i++) {
        entries[i] = rightVec.entries[i];
    }
    return *this;
}

template<int DIM>
vec<DIM> vec<DIM>::operator-(const vec<DIM>& rightVec) {
    vec<DIM> result;
    for(int i = 0; i < DIM; i++) {
        result.entries[i] = this->entries[i] - rightVec.entries[i];
    }
    return result;
}

template<int DIM>
template<int RDIM>
vec<DIM>& vec<DIM>::operator=( vec<RDIM>& rightVec){
    for(int i = 0; i < DIM; i++) {
        entries[i] = rightVec.entries[i];
    }
    return *this;
}

template<int DIM>
void vec<DIM>::normalize() { // divide by the length
    float norm = 0;
    for(int i = 0; i < DIM; i ++) { // add up the squares of entries
        norm += entries[i] * entries[i];
    }
    norm = sqrt(norm); // take square root to find length
    if(norm != 0) { // make sure no division by zero out there
        for(int i = 0; i < DIM; i++) entries[i] /= norm; // divide all entries by length
    }
}

/* operator <<: prints the entries of the vector to an ostream
 */
template <int DIM>
ostream& operator<<(ostream& os, const vec<DIM>& v) {
    for(int i = 0; i < DIM; i++) os << v[i] << " ";
    return os;
}


#endif // VEC_H
