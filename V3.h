
#pragma once

#include <ostream>
#include <istream>

using namespace std;

class V3 {
public:
    float xyz[3];
    V3() {};
    V3(float x, float y, float z);
    V3 operator+(V3 v1);
    V3 operator-(V3 v1);
    V3 operator/(float x);
    V3 operator*(float x);
    float operator*(V3 v1);
    float& operator[](int i);
    friend ostream& operator<<(ostream& ostr, V3 v);
    V3 operator^(V3 v1);
    V3 UnitVector();
    float Length();
    void SetFromColor(unsigned int col);
    V3 GetVectorColor(unsigned int col);
    unsigned int GetColor();
    V3 RotateThisPoint(V3 aO, V3 aD, float theta);
    V3 RotateThisVector(V3 aD, float theta);
    V3 Light(V3 matColor, float ka, V3 ld, V3 norm);
    V3 Reflect(V3 norm);
    V3 Specular(V3 vd, V3 rd, float specularExp, V3 matColor);
    void SetAsEdgeEquation(V3 p0, V3 p1, V3 p2);
};

class AABB {
public:
    V3 minv, maxv;
    AABB(V3 firstPoint);
    void AddPoint(V3 newPoint);
    int ClipWithFrame(int w, int h);
};