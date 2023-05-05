
#include "V3.h"
#include "M33.h"

using namespace std;

#include <ostream>
#include <math.h>
#include <iostream>

V3::V3(float x, float y, float z) {

    xyz[0] = x;
    xyz[1] = y;
    xyz[2] = z;

}

V3 V3::operator+(V3 v1) {

    V3 ret;
    V3& v0 = *this;
    ret[0] = v0[0] + v1[0];
    ret[1] = v0[1] + v1[1];
    ret[2] = v0[2] + v1[2];
    return ret;

}

V3 V3::operator-(V3 v1) {

    V3 ret;
    V3& v0 = *this;
    ret[0] = v0[0] - v1[0];
    ret[1] = v0[1] - v1[1];
    ret[2] = v0[2] - v1[2];
    return ret;

}

float& V3::operator[](int i) {

    return xyz[i];

}

float V3::operator*(V3 v1) {
    V3& v0 = *this;
    return v0[0] * v1[0] + v0[1] * v1[1] + v0[2] * v1[2];
}

ostream& operator<<(ostream& ostr, V3 v) {

    return ostr << v[0] << " " << v[1] << " " << v[2];

}

float V3::Length() {

    V3& v = *this;
    return sqrtf(v * v);

}

V3 V3::UnitVector() {

    V3 ret = *this;
    ret = ret / Length();
    return ret;
}


V3 V3::operator/(float scval) {

    V3 ret = *this;
    ret[0] /= scval;
    ret[1] /= scval;
    ret[2] /= scval;
    return ret;

}

V3 V3::operator*(float scval) {

    V3 ret = *this;
    ret[0] *= scval;
    ret[1] *= scval;
    ret[2] *= scval;
    return ret;

}

V3 V3::operator^(V3 v2) {

    V3& v1 = *this;
    float x1 = v1[0];
    float y1 = v1[1];
    float z1 = v1[2];
    float x2 = v2[0];
    float y2 = v2[1];
    float z2 = v2[2];
    return V3(y1 * z2 - z1 * y2, z1 * x2 - x1 * z2, x1 * y2 - y1 * x2);
}


void V3::SetFromColor(unsigned int col) {

    V3& v = *this;
    v[0] = (float)(((unsigned char*)(&col))[0]) / 255.0f;
    v[1] = (float)(((unsigned char*)(&col))[1]) / 255.0f;
    v[2] = (float)(((unsigned char*)(&col))[2]) / 255.0f;

}

V3 V3::GetVectorColor(unsigned int col) {

    V3& v = *this;
    v[0] = (float)(((unsigned char*)(&col))[0]) / 255.0f;
    v[1] = (float)(((unsigned char*)(&col))[1]) / 255.0f;
    v[2] = (float)(((unsigned char*)(&col))[2]) / 255.0f;
    return v;

}

unsigned int V3::GetColor() {

    V3 v = *this;
    v[0] = (v[0] < 0.0f) ? 0.0f : v[0];
    v[1] = (v[1] < 0.0f) ? 0.0f : v[1];
    v[2] = (v[2] < 0.0f) ? 0.0f : v[2];

    v[0] = (v[0] > 1.0f) ? 1.0f : v[0];
    v[1] = (v[1] > 1.0f) ? 1.0f : v[1];
    v[2] = (v[2] > 1.0f) ? 1.0f : v[2];

    unsigned int ret;
    int rgb[3];
    rgb[0] = (int)(v[0] * 255.0f);
    rgb[1] = (int)(v[1] * 255.0f);
    rgb[2] = (int)(v[2] * 255.0f);
    ret = 0xFF000000 + 256 * 256 * rgb[2] + 256 * rgb[1] + rgb[0];
    return ret;
}


V3 V3::RotateThisVector(V3 aD, float theta) {

    return RotateThisPoint(V3(0.0f, 0.0f, 0.0f), aD, theta);

}

V3 V3::RotateThisPoint(V3 aO, V3 aD, float theta) {

    M33 laxes;
    laxes[1] = aD;
    V3 aux(1.0f, 0.0f, 0.0f);
    if (fabsf(aD[0]) > fabsf(aD[1]))
        aux = V3(0.0f, 1.0f, 0.0f);
    laxes[2] = aux ^ aD;
    laxes[0] = laxes[1] ^ laxes[2];

    V3& P = *this;
    V3 Pl = laxes * (P - aO);
    M33 roty; roty.SetRotY(theta);
    V3 Pr = roty * Pl;
    return laxes.Inverted() * Pr + aO;

}

V3 V3::Light(V3 matColor, float ka, V3 ld, V3 norm) {
    V3 ret;
    float kd = (ld * -1.0f) * norm;
    kd = (kd < 0.0f) ? 0.0f : kd;
    ret = matColor * (ka + (1.0f - ka) * kd);
    return ret;
}

V3 V3::Reflect(V3 norm) {
    V3 ld = *this;
    V3 ret;
    float kd = max((ld * -1.0f) * norm, 0.0f);
    ret = ld - (norm * 2 * kd);
    return ret;
}

V3 V3::Specular(V3 vd, V3 rd, float specularExp, V3 matColor) {
    V3 ret;
    float spec = pow(max(vd * rd, 0.0f), 2);
    ret = matColor * specularExp * spec;
    return ret;
}


AABB::AABB(V3 firstPoint) {

    minv = maxv = firstPoint;

}

void AABB::AddPoint(V3 newPoint) {

    for (int ci = 0; ci < 3; ci++) {
        if (minv[ci] > newPoint[ci])
            minv[ci] = newPoint[ci];
        if (maxv[ci] < newPoint[ci])
            maxv[ci] = newPoint[ci];
    }

}

int AABB::ClipWithFrame(int w, int h) {

    if (minv[0] > (float)w)
        return 0;
    if (minv[1] > (float)h)
        return 0;
    if (maxv[0] < 0.0f)
        return 0;
    if (maxv[1] < 0.0f)
        return 0;

    if (minv[0] < 0.0f)
        minv[0] = 0.0f;
    if (minv[1] < 0.0f)
        minv[1] = 0.0f;
    if (maxv[0] > (float)w)
        maxv[0] = (float)w;
    if (maxv[1] > (float)h)
        maxv[1] = (float)h;

    return 1;

}

void V3::SetAsEdgeEquation(V3 p0, V3 p1, V3 p2) {

    // (x-x0)/(x1-x0) = (y-y0)/(y1-y0);
    // (x-x0)(y1-y0) = (y-y0)(x1-x0);
    // Ax+By+C = 0
    float x0 = p0[0], x1 = p1[0], y0 = p0[1], y1 = p1[1];
    float A, B, C;
    A = y1 - y0;
    B = x0 - x1;
    C = y0 * (x1 - x0) - x0 * (y1 - y0);
    V3& v = *this;
    v[0] = A;
    v[1] = B;
    v[2] = C;

    p2[2] = 1.0f;
    if (v * p2 < 0)
        v = v * -1.0f;

}