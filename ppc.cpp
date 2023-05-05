#include "ppc.h"
#include "M33.h"
#include <math.h>
#include <FL/Fl.H>
#include <FL/Fl_Gl_Window.H>
#include <GL/glut.h>
#include <FL/glu.h>

using namespace std;

#include <iostream>
#include <fstream>

PPC::PPC(float hfov, int _w, int _h) : w(_w), h(_h) {

    C = V3(0.0f, 0.0f, 0.0f);
    a = V3(1.0f, 0.0f, 0.0f);
    b = V3(0.0f, -1.0f, 0.0f);
    float hfovd = hfov / 180.0f * 3.1415926f;
    c = V3(-(float)w / 2.0f, (float)h / 2.0f, -(float)w / (2.0f * tan(hfovd / 2.0f)));

}

PPC PPC::PPCCopy(PPC* ppc, V3 newA, V3 newB, V3 newC) {
    PPC *newPPC = new PPC(60.0f, ppc->w, ppc->h);
    newPPC->a = ppc->a + newA;
    newPPC->b = ppc->b + newB;
    newPPC->c = ppc->c + newC;
    newPPC->C = ppc->C;

    return *newPPC;
}

V3 PPC::GetFBIntersection(int u, int v) {
    return a * (u + 0.5f) + b * (v + 0.5f) + c;
}


int PPC::Project(V3 P, V3& pP) {

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

V3 PPC::GetVD() {

    return (a ^ b).UnitVector();

}

void PPC::Rotate(V3 aDir, float theta) {

    a = a.RotateThisVector(aDir, theta);
    b = b.RotateThisVector(aDir, theta);
    c = c.RotateThisVector(aDir, theta);

}

float PPC::GetF() {

    return c * GetVD();

}

void PPC::PositionAndOrient(V3 Cn, V3 L, V3 upg) {

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

void PPC::Pan(float theta) {
    a = a.RotateThisVector(b.UnitVector() * -1.0f, theta);
    b = b.RotateThisVector(b.UnitVector() * -1.0f, theta);
    c = c.RotateThisVector(b.UnitVector() * -1.0f, theta);
}

void PPC::Tilt(float theta) {
    a = a.RotateThisVector(a.UnitVector(), theta);
    b = b.RotateThisVector(a.UnitVector(), theta);
    c = c.RotateThisVector(a.UnitVector(), theta);
}

void PPC::Roll(float theta) {
    a = a.RotateThisVector(a.UnitVector() ^ b.UnitVector(), theta);
    b = b.RotateThisVector(a.UnitVector() ^ b.UnitVector(), theta);
    c = c.RotateThisVector(a.UnitVector() ^ b.UnitVector(), theta);
    //std::cout << C << std::endl;
}

void PPC::TranslateLR(float step) {
    C = C + a.UnitVector() * step;
}

void PPC::TranslateUD(float step) {
    C = C - b.UnitVector() * step;
}

void PPC::TranslateFB(float step) {
    C = C + (a ^ b).UnitVector() * step;
}

void PPC::Zoom(float zoom) {
    float ppu = (c * (-1)) * a.UnitVector() / a.Length();
    float ppv = (c * (-1)) * b.UnitVector() / b.Length();
    float f = GetF() * zoom;
    c = a * (-1) * ppu - b * ppv + GetVD() * f;
}

void PPC::Unproject(V3 pP, V3& P) {

    P = C + (a * pP[0] + b * pP[1] + c) / pP[2];

}

void PPC::SetIntrinsicsHW(float nearz, float farz) {

    glViewport(0, 0, w, h);
    float scf = nearz / GetF();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-(float)w / 2.0f * scf, (float)w / 2.0f * scf, -(float)h / 2.0f * scf, (float)h / 2.0f * scf,
        nearz, farz);

    glMatrixMode(GL_MODELVIEW);

}


