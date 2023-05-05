#pragma once

#include "V3.h"
#include "M33.h"

class PPC {
public:
    V3 a, b, c, C;
    int w, h;
    PPC(float hfov, int _w, int _h);
    PPC(V3 _a, V3 _b, V3 _c, V3 _C);
    PPC PPCCopy(PPC* ppc, V3 newA, V3 newB, V3 newC);
    V3 GetFBIntersection(int u, int v);
    int Project(V3 P, V3& pP);
    void Unproject(V3 pP, V3& P);
    V3 GetVD();
    void Rotate(V3 aDir, float theta);
    void PositionAndOrient(V3 Cn, V3 L, V3 upg);
    void Pan(float theta);
    void Tilt(float theta);
    void Roll(float theta);
    void TranslateLR(float step);
    void TranslateUD(float step);
    void TranslateFB(float step);
    void Zoom(float zoom);
    float GetF();
    void SetIntrinsicsHW(float nearz, float farz);
    void SetExtrinsicsHW();
    M33 GetMatrix();
    void SaveFile(string filename);
    void LoadFile(string filename, int index);
    V3 StringToV3(string line);
};