#pragma once

#include <FL/Fl.H>
#include <FL/Fl_Gl_Window.H>
#include <GL/glut.h>

#include "V3.h"
#include "M33.h"

class PPC;

class FrameBuffer : public Fl_Gl_Window {
public:
	struct sample_ll {
		float u;
		float v;
		float z;
		float u0;
		float v0;
		sample_ll* next;
	};

	unsigned int *pix; // pixel array
	float *zb; // z buffer to resolve visibility
	float *sm; // shadow map
	sample_ll **izb; // irregular z buffer for shadows
	unsigned int* panomap; // map to another fb
	int w, h;
	FrameBuffer(int u0, int v0, int _w, int _h);
	void CopyFB(FrameBuffer* fb);
	void draw();
	int handle(int event);
	void KeyboardHandle();
	void SetBGR(unsigned int bgr);
	void Set(int u, int v, int col);
	int GetCol(int u, int v);
	void SetGuarded(int u, int v, int col);
	void SetChecker(unsigned int col0, unsigned int col1, int csize);
	void DrawAARectangle(V3 tlc, V3 brc, unsigned int col);
	void DrawDisk(V3 center, float r, unsigned int col);
	void DrawSegment(V3 p0, V3 p1, unsigned int col);
	void DrawSegment(V3 p0, V3 p1, V3 c0, V3 c1);
	void Render3DSegment(PPC *ppc, V3 v0, V3 v1, V3 c0, V3 c1);
	void Render3DPoint(PPC *ppc, V3 p, V3 c, float psize);
	void DrawTriangle(float x[3], float y[3], float z[3], V3 n[3], V3 matColor, V3 ld, float ka, V3 ambient, float specularExp, V3 vd);
	void RenderFill(float x[3], float y[3], float z[3], V3 n[3], V3 matColor, V3 ld, float ka, V3 ambient, float specularExp, V3 vd, PPC* ppc, class Light* light);
	void UpdateIZB(PPC* ppc, Light* light);
	void UpdateIZB2(V3 pvs[3], V3 ns[3], V3 cs[3], V3 matColor, V3 ld, float ka, V3 ambient, float specularExp, V3 vd, PPC* ppc, class Light* light);
	void DrawTriangleWithShadow(float x[3], float y[3], float z[3], V3 n[3], V3 matColor, V3 ld, float ka, V3 ambient, float specularExp, V3 vd, PPC* ppc, class Light* light);
	void LoadTiff(char* fname);
	void SaveAsTiff(char* fname);
	void SetZB(float zf);
	void SetSM(float sf);
	void SetShadow(V3 pvs[3]);
	void DrawIZBShadow(float x[3], float y[3], float z[3]);
	void DrawIZBShadow2(V3 pvs[3]);
	int IsCloserThenSet(float currz, int u, int v);
	int IsCloserThenSetSM(float currz, int u, int v);
	void ViewShadowMap(float* some_sm);
	void PrintIZB();
	void ViewIZB(sample_ll **some_izb);
	void ResetIZB();
	unsigned int MakeColDarker(unsigned int col, float shade);
	void SetTextureImage(char* fname);
	void GetSpherical(PPC* ppc, FrameBuffer* panorama);
	void GetSpherical(PPC* ppc, FrameBuffer* panorama, V3 gpCenter, class Light* light);
	int LookupSpherical(PPC* ppc, FrameBuffer* panorama, V3 ray);
	unsigned int Get(int u, int v);
	void SetPanoMap(int u, int v);
	void RenderPanoMap();
	void ResetPanoMap();
};
