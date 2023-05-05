#pragma once

#include "V3.h"

class Light {
public:
    V3 a, b, c, C;
    int w, h;
    Light(float hfov, int _w, int _h);
    int Project(V3 P, V3& pP);
    void Unproject(V3 pP, V3& P);
    V3 GetVD();
    void Rotate(V3 aDir, float theta);
    void PositionAndOrient(V3 Cn, V3 L, V3 upg);
    void TranslateLR(float step);
    void TranslateUD(float step);
    void TranslateFB(float step);
    void Pan(float theta);
    float GetF();
};