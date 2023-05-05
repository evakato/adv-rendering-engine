#include "light.h"
#include "M33.h"
#include <math.h>


Light::Light(float hfov, int _w, int _h) : w(_w), h(_h) {

    C = V3(0.0f, 0.0f, 0.0f);
    a = V3(1.0f, 0.0f, 0.0f);
    b = V3(0.0f, -1.0f, 0.0f);
    float hfovd = hfov / 180.0f * 3.1415926f;
    c = V3(-(float)w / 2.0f, (float)h / 2.0f, -(float)w / (2.0f * tan(hfovd / 2.0f)));

}

int Light::Project(V3 P, V3& pP) {

    M33 M;
    M.SetColumn(0, a);
    M.SetColumn(1, b);
    M.SetColumn(2, c);
    V3 q = M.Inverted() * (P - C);
    if (q[2] <= 0.0f)
        return 0;

    pP[0] = q[0] / q[2];
    pP[1] = q[1] / q[2];
    pP[2] = 1.0f / q[2];
    return 1;

}

void Light::Unproject(V3 pP, V3& P) {

    P = C + (a * pP[0] + b * pP[1] + c) / pP[2];

}

V3 Light::GetVD() {
    return (a ^ b).UnitVector();
}

float Light::GetF() {
    return c * GetVD();
}

void Light::Rotate(V3 aDir, float theta) {

    a = a.RotateThisVector(aDir, theta);
    b = b.RotateThisVector(aDir, theta);
    c = c.RotateThisVector(aDir, theta);

}

void Light::PositionAndOrient(V3 Cn, V3 L, V3 upg) {

    V3 vdn = (L - Cn).UnitVector();
    V3 an = (vdn ^ upg).UnitVector();
    V3 bn = vdn ^ an;
    float f = GetF();
    V3 cn = vdn * f - an * (float)w / 2.0f - bn * (float)h / 2.0f;

    a = an;
    b = bn;
    c = cn;
    C = Cn;

}

void Light::TranslateLR(float step) {
    C = C + a.UnitVector() * step;
}

void Light::TranslateUD(float step) {
    C = C - b.UnitVector() * step;
}

void Light::TranslateFB(float step) {
    C = C + (a ^ b).UnitVector() * step;
}

void Light::Pan(float theta) {
    a = a.RotateThisVector(b.UnitVector() * -1.0f, theta);
    b = b.RotateThisVector(b.UnitVector() * -1.0f, theta);
    c = c.RotateThisVector(b.UnitVector() * -1.0f, theta);
}