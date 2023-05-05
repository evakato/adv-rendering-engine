#pragma once

#include "V3.h"

class PPC;
class FrameBuffer;
class Light;

class TM {
public:
	V3 *verts;
	int vertsN;
	V3 *cols, *normals;
	unsigned int *tris;
	int trisN;
	TM() : verts(0), vertsN(0), tris(0), trisN(0), cols(0), normals(0) {};
	void SetOneTriangle(V3 v0, V3 v1, V3 v2);
	void SetAsAACube(V3 cc, float sideLength);
	void SetFullCube(V3 cc, float sideLength);
	void SetPlane(V3 cc, float length, float width);
	void LoadBin(char *fname);
	void RenderAsPoints(int psize, PPC *ppc, FrameBuffer *fb);
	void RenderWireFrame(PPC *ppc, FrameBuffer *fb);
	void RenderShadowMap(Light* light, FrameBuffer* fb);
	void RenderFilled(PPC *ppc, FrameBuffer *fb);
	void RenderZBOnly(PPC* ppc, FrameBuffer* fb, V3 gpCenter);
	void RenderFillInWater(PPC *ppc, FrameBuffer *fb, FrameBuffer *waterFb, V3 gpCenter);
	void RenderFillWithReflection(PPC *ppc, FrameBuffer *fb, FrameBuffer *panorama);
	void RenderFill(PPC* ppc, FrameBuffer* fb, V3 matColor, V3 ld, float ka, float ambientFactor, float specularExp);
	void RenderFillWithShadow(PPC* ppc, FrameBuffer* fb, V3 matColor, V3 ld, float ka, float ambientFactor, float specularExp, class Light* light);
	void RenderUpdateIZB(PPC* ppc, FrameBuffer* fb, V3 matColor, V3 ld, float ka, float ambientFactor, float specularExp, class Light* light);
	void RenderIrregularShadows(class Light* light, FrameBuffer* fb);
	void RenderFillShadow(PPC* ppc, FrameBuffer* fb, class Light* light);
	V3 GetCenter();
	void Translate(V3 tv);
	void SetCenter(V3 newCenter);
	void Rotate(V3 aO, V3 aD, float theta);
	void Light(V3 matColor, float ka, V3 ld);
	void VisualizeVertexNormals(PPC *ppc, FrameBuffer *fb, float vlength);
	void SetUnshared(TM *tm);
	void SetAsCopy(TM *tm);
	void Explode(float t);
	void ReflectVertices(V3 gpCenter, TM ogMesh);
	void ScaleMesh(V3 sv);
};