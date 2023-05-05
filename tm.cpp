#include "tm.h"
#include "ppc.h"
#include "framebuffer.h"
#include "M33.h"
#include "light.h"

using namespace std;

#include <fstream>
#include <iostream>

const int sm_height = 200;
const int sm_width = 200;

void TM::SetFullCube(V3 cc, float sideLength) {

	vertsN = 24;
	verts = new V3[vertsN];
	normals = new V3[vertsN];
	float sl2 = sideLength / 2.0f;

	verts[0] = cc + V3(-sl2, +sl2, +sl2);
	verts[1] = cc + V3(-sl2, -sl2, +sl2);
	verts[2] = cc + V3(+sl2, -sl2, +sl2);
	verts[3] = cc + V3(+sl2, +sl2, +sl2);
	normals[0] = V3(0.0f, 0.0f, 1.0f);
	normals[1] = V3(0.0f, 0.0f, 1.0f);
	normals[2] = V3(0.0f, 0.0f, 1.0f);
	normals[3] = V3(0.0f, 0.0f, 1.0f);

	verts[4] = cc + V3(-sl2, +sl2, -sl2);
	verts[5] = cc + V3(-sl2, -sl2, -sl2);
	verts[6] = cc + V3(+sl2, -sl2, -sl2);
	verts[7] = cc + V3(+sl2, +sl2, -sl2);
	normals[4] = V3(0.0f, 0.0f, -1.0f);
	normals[5] = V3(0.0f, 0.0f, -1.0f);
	normals[6] = V3(0.0f, 0.0f, -1.0f);
	normals[7] = V3(0.0f, 0.0f, -1.0f);

	verts[8] = cc + V3(+sl2, -sl2, -sl2); // 6
	verts[9] = cc + V3(+sl2, +sl2, -sl2); // 7
	verts[10] = cc + V3(+sl2, -sl2, +sl2); // 2
	verts[11] = cc + V3(+sl2, +sl2, +sl2); // 3
	normals[8] = V3(1.0f, 0.0f, 0.0f);
	normals[9] = V3(1.0f, 0.0f, 0.0f);
	normals[10] = V3(1.0f, 0.0f, 0.0f);
	normals[11] = V3(1.0f, 0.0f, .0f);

	verts[12] = cc + V3(-sl2, +sl2, +sl2); // 0
	verts[13] = cc + V3(-sl2, -sl2, +sl2); // 1
	verts[14] = cc + V3(-sl2, +sl2, -sl2); // 4
	verts[15] = cc + V3(-sl2, -sl2, -sl2); // 5
	normals[12] = V3(-1.0f, 0.0f, 0.0f);
	normals[13] = V3(-1.0f, 0.0f, 0.0f);
	normals[14] = V3(-1.0f, 0.0f, 0.0f);
	normals[15] = V3(-1.0f, 0.0f, .0f);

	verts[16] = cc + V3(-sl2, +sl2, +sl2);
	verts[17] = cc + V3(+sl2, +sl2, +sl2);
	verts[18] = cc + V3(-sl2, +sl2, -sl2);
	verts[19] = cc + V3(+sl2, +sl2, -sl2);
	normals[16] = V3(0.0f, 1.0f, 0.0f);
	normals[17] = V3(0.0f, 1.0f, 0.0f);
	normals[18] = V3(0.0f, 1.0f, 0.0f);
	normals[19] = V3(.0f, 1.0f, .0f);

	verts[20] = cc + V3(-sl2, -sl2, +sl2);
	verts[21] = cc + V3(+sl2, -sl2, +sl2);
	verts[22] = cc + V3(-sl2, -sl2, -sl2);
	verts[23] = cc + V3(+sl2, -sl2, -sl2);
	normals[20] = V3(0.0f, -1.0f, 0.0f);
	normals[21] = V3(0.0f, -1.0f, 0.0f);
	normals[22] = V3(0.0f, -1.0f, 0.0f);
	normals[23] = V3(.0f, -1.0f, .0f);

	trisN = 12;
	tris = new unsigned int[trisN*3];
	int tri = 0;
	tris[3 * tri + 0] = 0; // tris[0] = 0;
	tris[3 * tri + 1] = 1;
	tris[3 * tri + 2] = 2;
	tri++;
	tris[3 * tri + 0] = 2;
	tris[3 * tri + 1] = 3;
	tris[3 * tri + 2] = 0;
	tri++;

	tris[3 * tri + 0] = 11;
	tris[3 * tri + 1] = 10;
	tris[3 * tri + 2] = 8;
	tri++;
	tris[3 * tri + 0] = 8;
	tris[3 * tri + 1] = 9;
	tris[3 * tri + 2] = 11;
	tri++;

	tris[3 * tri + 0] = 7;
	tris[3 * tri + 1] = 6;
	tris[3 * tri + 2] = 5;
	tri++;
	tris[3 * tri + 0] = 5;
	tris[3 * tri + 1] = 4;
	tris[3 * tri + 2] = 7;
	tri++;

	tris[3 * tri + 0] = 14;
	tris[3 * tri + 1] = 15;
	tris[3 * tri + 2] = 13;
	tri++;
	tris[3 * tri + 0] = 13;
	tris[3 * tri + 1] = 12;
	tris[3 * tri + 2] = 14;
	tri++;


	tris[3 * tri + 0] = 20;
	tris[3 * tri + 1] = 22;
	tris[3 * tri + 2] = 23;
	tri++;
	tris[3 * tri + 0] = 23;
	tris[3 * tri + 1] = 21;
	tris[3 * tri + 2] = 20;
	tri++;

	tris[3 * tri + 0] = 18;
	tris[3 * tri + 1] = 16;
	tris[3 * tri + 2] = 17;
	tri++;
	tris[3 * tri + 0] = 17;
	tris[3 * tri + 1] = 19;
	tris[3 * tri + 2] = 18;
	tri++;

	cols = new V3[vertsN];
	for (int i = 0; i < 4; i++) {
			cols[i] = V3(1.0f, 0.0f, 0.0f);
	}
	for (int i = 4; i < 8; i++) {
			cols[i] = V3(0.0f, 1.0f, 0.0f);
	}
	for (int i = 8; i < 12; i++) {
			cols[i] = V3(0.0f, 0.0f, 1.0f);
	}
	for (int i = 12; i < 16; i++) {
			cols[i] = V3(1.0f, 1.0f, 0.0f);
	}
	for (int i = 16; i < 20; i++) {
			cols[i] = V3(1.0f, 0.0f, 1.0f);
	}
	for (int i = 20; i < 24; i++) {
			cols[i] = V3(0.0f, 1.0f, 1.0f);
	}
}

void TM::LoadBin(char *fname) {

		ifstream ifs(fname, ios::binary);
		if (ifs.fail()) {
			cerr << "INFO: cannot open file: " << fname << endl;
			return;
		}

		ifs.read((char*)&vertsN, sizeof(int));
		char yn;
		ifs.read(&yn, 1); // always xyz
		if (yn != 'y') {
			cerr << "INTERNAL ERROR: there should always be vertex xyz data" << endl;
			return;
		}
		if (verts)
			delete verts;
		verts = new V3[vertsN];

		ifs.read(&yn, 1); // cols 3 floats
		normals = 0;
		if (cols)
			delete cols;
		cols = 0;
		if (yn == 'y') {
			cols = new V3[vertsN];
		}

		ifs.read(&yn, 1); // normals 3 floats
		if (normals)
			delete normals;
		normals = 0;
		if (yn == 'y') {
			normals = new V3[vertsN];
		}

		ifs.read(&yn, 1); // texture coordinates 2 floats
		float *tcs = 0; // don't have texture coordinates for now
		if (tcs)
			delete tcs;
		tcs = 0;
		if (yn == 'y') {
			tcs = new float[vertsN * 2];
		}


		ifs.read((char*)verts, vertsN * 3 * sizeof(float)); // load verts

		if (cols) {
			ifs.read((char*)cols, vertsN * 3 * sizeof(float)); // load cols
		}

		if (normals)
			ifs.read((char*)normals, vertsN * 3 * sizeof(float)); // load normals

		if (tcs)
			ifs.read((char*)tcs, vertsN * 2 * sizeof(float)); // load texture coordinates

	ifs.read((char*)&trisN, sizeof(int));
	if (tris)
		delete tris;
	tris = new unsigned int[trisN * 3];
	ifs.read((char*)tris, trisN * 3 * sizeof(unsigned int)); // read tiangles

	ifs.close();

	cerr << "INFO: loaded " << vertsN << " verts, " << trisN << " tris from " << endl << "      " << fname << endl;
	cerr << "      xyz " << ((cols) ? "rgb " : "") << ((normals) ? "nxnynz " : "") << ((tcs) ? "tcstct " : "") << endl;

}

void TM::RenderAsPoints(int psize, PPC *ppc, FrameBuffer *fb) {

	for (int vi = 0; vi < vertsN; vi++) {
		V3 pp;
		if (!ppc->Project(verts[vi], pp))
			continue;
		fb->DrawDisk(pp, (float)psize, 0xFF0000FF);
	}

}

void TM::RenderWireFrame(PPC *ppc, FrameBuffer *fb) {

	for (int tri = 0; tri < trisN; tri++) {
		for (int ei = 0; ei < 3; ei++) {
			V3 v0 = verts[tris[3 * tri + ei]];
			V3 v1 = verts[tris[3 * tri + (ei + 1) % 3]];
			V3 c0 = cols[tris[3 * tri + ei]];
			V3 c1 = cols[tris[3 * tri + (ei + 1) % 3]];
			V3 pv0, pv1;
			if (!ppc->Project(v0, pv0))
				continue;
			if (!ppc->Project(v1, pv1))
				continue;
			fb->DrawSegment(pv0, pv1, c0, c1);
		}
	}

}

void TM::RenderShadowMap(class Light* light, FrameBuffer* fb) {
	for (int tri = 0; tri < trisN; tri++) {
		V3 vs[3];
		for (int vi = 0; vi < 3; vi++) {
			vs[vi] = verts[tris[3 * tri + vi]];
		}

		// project vertices
		V3 pvs[3];
		int needContinue = 0;
		for (int vi = 0; vi < 3; vi++) {
			if (!light->Project(vs[vi], pvs[vi])) {
				needContinue = 1;
				break;
			}
		}
		if (needContinue)
			continue;

		fb->SetShadow(pvs);
	}
}

void TM::RenderFilled(PPC *ppc, FrameBuffer *fb) {

	for (int tri = 0; tri < trisN; tri++) {
		V3 vs[3], cs[3], ns[3];
		for (int vi = 0; vi < 3; vi++) {
			vs[vi] = verts[tris[3 * tri + vi]];
			cs[vi] = cols[tris[3 * tri + vi]];
			ns[vi] = normals[tris[3 * tri + vi]];
		}
		V3 pvs[3];
		int needContinue = 0;
		for (int vi = 0; vi < 3; vi++) {
			if (!ppc->Project(vs[vi], pvs[vi])) {
				needContinue = 1;
				break;
			}
		}
		if (needContinue)
			continue;

		AABB aabb(pvs[0]);
		aabb.AddPoint(pvs[1]);
		aabb.AddPoint(pvs[2]);
		if (!aabb.ClipWithFrame(fb->w, fb->h))
			continue;

		M33 eeqs;
		eeqs[0].SetAsEdgeEquation(pvs[0], pvs[1], pvs[2]);
		eeqs[1].SetAsEdgeEquation(pvs[1], pvs[2], pvs[0]);
		eeqs[2].SetAsEdgeEquation(pvs[2], pvs[0], pvs[1]);
		M33 ssim; ssim[0] = pvs[0]; ssim[1] = pvs[1]; ssim[2] = pvs[2];
		ssim.SetColumn(2, V3(1.0f, 1.0f, 1.0f)); ssim = ssim.Inverted();
		V3 le1w = ssim * V3(pvs[0][2], pvs[1][2], pvs[2][2]);
		M33 colm; colm[0] = cs[0]; colm[1] = cs[1]; colm[2] = cs[2];
		M33 lecols = (ssim * colm).Transposed();
		M33 nm; nm[0] = ns[0]; nm[1] = ns[1]; nm[2] = ns[2];
		M33 lenormals = (ssim * nm).Transposed();
		int top = (int)(aabb.minv[1] + 0.5f);
		int bottom = (int)(aabb.maxv[1] - 0.5f);
		int left = (int)(aabb.minv[0] + 0.5f);
		int right = (int)(aabb.maxv[0] - 0.5f);
		for (int v = top; v <= bottom; v++) {
			for (int u = left; u <= right; u++) {
				V3 pixv(.5f + (float)u, .5f + (float)v, 1.0f);
				V3 svs = eeqs*pixv;
				if (svs[0] < 0.0f || svs[1] < 0.0f || svs[2] < 0.0f)
					continue;
				float currz = le1w * pixv;
				if (fb->IsCloserThenSet(currz, u, v)) {
					V3 currCol = lecols*pixv;
					//V3 currNormal = (lenormals*pixv).UnitVector();
					//currCol = currCol.Light(V3(1.0f, 0.0f, 0.0f), 0.1f, 
						//V3(0.0f, 0.0f, -1.0f), currNormal);
					fb->Set(u, v, currCol.GetColor());
				}
			}
		}
	}
}

void TM::RenderZBOnly(PPC* ppc, FrameBuffer* fb, V3 gpCenter) {

	for (int tri = 0; tri < trisN; tri++) {
		V3 vs[3], cs[3], ns[3];
		for (int vi = 0; vi < 3; vi++) {
			vs[vi] = verts[tris[3 * tri + vi]];
			cs[vi] = cols[tris[3 * tri + vi]];
			ns[vi] = normals[tris[3 * tri + vi]];
		}
		V3 pvs[3];
		int needContinue = 0;
		for (int vi = 0; vi < 3; vi++) {
			if (!ppc->Project(vs[vi], pvs[vi])) {
				needContinue = 1;
				break;
			}
		}
		if (needContinue)
			continue;

		AABB aabb(pvs[0]);
		aabb.AddPoint(pvs[1]);
		aabb.AddPoint(pvs[2]);
		if (!aabb.ClipWithFrame(fb->w, fb->h))
			continue;

		M33 eeqs;
		eeqs[0].SetAsEdgeEquation(pvs[0], pvs[1], pvs[2]);
		eeqs[1].SetAsEdgeEquation(pvs[1], pvs[2], pvs[0]);
		eeqs[2].SetAsEdgeEquation(pvs[2], pvs[0], pvs[1]);
		M33 ssim; ssim[0] = pvs[0]; ssim[1] = pvs[1]; ssim[2] = pvs[2];
		ssim.SetColumn(2, V3(1.0f, 1.0f, 1.0f)); ssim = ssim.Inverted();
		V3 le1w = ssim * V3(pvs[0][2], pvs[1][2], pvs[2][2]);
		M33 colm; colm[0] = cs[0]; colm[1] = cs[1]; colm[2] = cs[2];
		M33 lecols = (ssim * colm).Transposed();
		M33 nm; nm[0] = ns[0]; nm[1] = ns[1]; nm[2] = ns[2];
		M33 lenormals = (ssim * nm).Transposed();
		int top = (int)(aabb.minv[1] + 0.5f);
		int bottom = (int)(aabb.maxv[1] - 0.5f);
		int left = (int)(aabb.minv[0] + 0.5f);
		int right = (int)(aabb.maxv[0] - 0.5f);
		for (int v = top; v <= bottom; v++) {
			for (int u = left; u <= right; u++) {
				V3 pixv(.5f + (float)u, .5f + (float)v, 1.0f);
				V3 svs = eeqs * pixv;
				if (svs[0] < 0.0f || svs[1] < 0.0f || svs[2] < 0.0f)
					continue;
				float currz = le1w * pixv;
				V3 unproject;
				ppc->Unproject(V3(pixv[0], pixv[1], currz), unproject);
				if (unproject[2] >= gpCenter[2]) {
					fb->IsCloserThenSet(currz, u, v);
				}
			}
		}
	}
}

void TM::RenderFillInWater(PPC* ppc, FrameBuffer* fb, FrameBuffer* waterFb, V3 gpCenter) {

	for (int tri = 0; tri < trisN; tri++) {
		V3 vs[3], cs[3], ns[3];
		for (int vi = 0; vi < 3; vi++) {
			vs[vi] = verts[tris[3 * tri + vi]];
			cs[vi] = cols[tris[3 * tri + vi]];
			ns[vi] = normals[tris[3 * tri + vi]];
		}
		V3 pvs[3];
		int needContinue = 0;
		for (int vi = 0; vi < 3; vi++) {
			if (!ppc->Project(vs[vi], pvs[vi])) {
				needContinue = 1;
				break;
			}
		}
		if (needContinue)
			continue;

		AABB aabb(pvs[0]);
		aabb.AddPoint(pvs[1]);
		aabb.AddPoint(pvs[2]);
		if (!aabb.ClipWithFrame(fb->w, fb->h))
			continue;

		M33 eeqs;
		eeqs[0].SetAsEdgeEquation(pvs[0], pvs[1], pvs[2]);
		eeqs[1].SetAsEdgeEquation(pvs[1], pvs[2], pvs[0]);
		eeqs[2].SetAsEdgeEquation(pvs[2], pvs[0], pvs[1]);
		M33 ssim; ssim[0] = pvs[0]; ssim[1] = pvs[1]; ssim[2] = pvs[2];
		ssim.SetColumn(2, V3(1.0f, 1.0f, 1.0f)); ssim = ssim.Inverted();
		V3 le1w = ssim * V3(pvs[0][2], pvs[1][2], pvs[2][2]);
		M33 colm; colm[0] = cs[0]; colm[1] = cs[1]; colm[2] = cs[2];
		M33 lecols = (ssim * colm).Transposed();
		M33 nm; nm[0] = ns[0]; nm[1] = ns[1]; nm[2] = ns[2];
		M33 lenormals = (ssim * nm).Transposed();
		int top = (int)(aabb.minv[1] + 0.5f);
		int bottom = (int)(aabb.maxv[1] - 0.5f);
		int left = (int)(aabb.minv[0] + 0.5f);
		int right = (int)(aabb.maxv[0] - 0.5f);
		for (int v = top; v <= bottom; v++) {
			for (int u = left; u <= right; u++) {
				V3 pixv(.5f + (float)u, .5f + (float)v, 1.0f);
				V3 svs = eeqs * pixv;
				if (svs[0] < 0.0f || svs[1] < 0.0f || svs[2] < 0.0f)
					continue;
				float currz = le1w * pixv;
				V3 unproject;
				ppc->Unproject(V3((float)u, (float)v, currz), unproject);
				if (unproject[2] <= gpCenter[2]) {
					if (fb->IsCloserThenSet(currz, u, v)) {
							V3 currCol = lecols * pixv;
							V3 waterCol;
							waterCol = waterCol.GetVectorColor(waterFb->GetCol(u, v));
							// box
							//fb->Set(u, v, ((currCol * 1.5f) + waterCol).GetColor());
							fb->Set(u, v, ((currCol) + (waterCol*0.7f)).GetColor());
					}
				}
			}
		}
	}
}

void TM::RenderFillWithReflection(PPC* ppc, FrameBuffer* fb, FrameBuffer* panorama) {

	for (int tri = 0; tri < trisN; tri++) {
		V3 vs[3], cs[3], ns[3];
		for (int vi = 0; vi < 3; vi++) {
			vs[vi] = verts[tris[3 * tri + vi]];
			cs[vi] = cols[tris[3 * tri + vi]];
			ns[vi] = normals[tris[3 * tri + vi]];
		}
		V3 pvs[3];
		int needContinue = 0;
		for (int vi = 0; vi < 3; vi++) {
			if (!ppc->Project(vs[vi], pvs[vi])) {
				needContinue = 1;
				break;
			}
		}
		if (needContinue)
			continue;

		AABB aabb(pvs[0]);
		aabb.AddPoint(pvs[1]);
		aabb.AddPoint(pvs[2]);
		if (!aabb.ClipWithFrame(fb->w, fb->h))
			continue;

		M33 eeqs;
		eeqs[0].SetAsEdgeEquation(pvs[0], pvs[1], pvs[2]);
		eeqs[1].SetAsEdgeEquation(pvs[1], pvs[2], pvs[0]);
		eeqs[2].SetAsEdgeEquation(pvs[2], pvs[0], pvs[1]);
		M33 ssim; ssim[0] = pvs[0]; ssim[1] = pvs[1]; ssim[2] = pvs[2];
		ssim.SetColumn(2, V3(1.0f, 1.0f, 1.0f)); ssim = ssim.Inverted();
		V3 le1w = ssim * V3(pvs[0][2], pvs[1][2], pvs[2][2]);
		M33 colm; colm[0] = cs[0]; colm[1] = cs[1]; colm[2] = cs[2];
		M33 lecols = (ssim * colm).Transposed();
		M33 nm; nm[0] = ns[0]; nm[1] = ns[1]; nm[2] = ns[2];
		M33 lenormals = (ssim * nm).Transposed();
		int top = (int)(aabb.minv[1] + 0.5f);
		int bottom = (int)(aabb.maxv[1] - 0.5f);
		int left = (int)(aabb.minv[0] + 0.5f);
		int right = (int)(aabb.maxv[0] - 0.5f);
		for (int v = top; v <= bottom; v++) {
			for (int u = left; u <= right; u++) {
				V3 pixv(.5f + (float)u, .5f + (float)v, 1.0f);
				V3 svs = eeqs * pixv;
				if (svs[0] < 0.0f || svs[1] < 0.0f || svs[2] < 0.0f)
					continue;
				float currz = le1w * pixv;
				if (fb->IsCloserThenSet(currz, u, v)) {
					V3 currCol = lecols * pixv;
					V3 currNormal = (lenormals*pixv).UnitVector();
					if (currNormal[2] <= -0.90f) {
						V3 reflectedV = ((ppc->GetFBIntersection(u, v)).Reflect(currNormal)).UnitVector();
						V3 lookupSphere;
						lookupSphere.SetFromColor(fb->LookupSpherical(ppc, panorama, reflectedV));
						//currCol = currCol.Light(V3(1.0f, 0.0f, 0.0f), 0.1f, V3(0.0f, 0.0f, -1.0f), currNormal);
						currCol = lookupSphere/2.0f + currCol/4.0f;
					}
					fb->Set(u, v, currCol.GetColor());
				}
			}
		}
	}
}

// Filled mode render with Phong shading instead of color interpolation
void TM::RenderFill(PPC* ppc, FrameBuffer* fb, V3 matColor, V3 ld, float ka, float ambientFactor, float specularExp) {
	for (int tri = 0; tri < trisN; tri++) {
		// get vertices and normals
		V3 vs[3]; V3 ns[3];
		for (int vi = 0; vi < 3; vi++) {
			vs[vi] = verts[tris[3 * tri + vi]];
			ns[vi] = normals[tris[3 * tri + vi]];
		}

		// project vertices
		V3 pvs[3];
		int needContinue = 0;
		for (int vi = 0; vi < 3; vi++) {
			if (!ppc->Project(vs[vi], pvs[vi])) {
				needContinue = 1;
				break;
			}
		}
		if (needContinue)
			continue;

		float x[3]; x[0] = pvs[0][0]; x[1] = pvs[1][0]; x[2] = pvs[2][0];
		float y[3]; y[0] = pvs[0][1]; y[1] = pvs[1][1]; y[2] = pvs[2][1];
		float z[3]; z[0] = pvs[0][2]; z[1] = pvs[1][2]; z[2] = pvs[2][2];
		V3 ambient = matColor * ambientFactor;

		fb->DrawTriangle(x, y, z, ns, matColor, ld, ka, ambient, specularExp, ppc->GetVD());
	}
}

// Update IZB
void TM::RenderUpdateIZB(PPC* ppc, FrameBuffer* fb, V3 matColor, V3 ld, float ka,
	float ambientFactor, float specularExp, class Light* light) {

	for (int tri = 0; tri < trisN; tri++) {
		// get vertices and normals
		V3 vs[3]; V3 ns[3]; V3 cs[3];
		for (int vi = 0; vi < 3; vi++) {
			vs[vi] = verts[tris[3 * tri + vi]];
			ns[vi] = normals[tris[3 * tri + vi]];
			cs[vi] = cols[tris[3 * tri + vi]];
		}

		V3 newMatColor = cs[0];

		// project vertices
		V3 pvs[3];
		int needContinue = 0;
		for (int vi = 0; vi < 3; vi++) {
			if (!ppc->Project(vs[vi], pvs[vi])) {
				needContinue = 1;
				break;
			}
		}
		if (needContinue)
			continue;

		float x[3]; x[0] = pvs[0][0]; x[1] = pvs[1][0]; x[2] = pvs[2][0];
		float y[3]; y[0] = pvs[0][1]; y[1] = pvs[1][1]; y[2] = pvs[2][1];
		float z[3]; z[0] = pvs[0][2]; z[1] = pvs[1][2]; z[2] = pvs[2][2];
		V3 ambient = matColor * ambientFactor;

		fb->RenderFill(x, y, z, ns, newMatColor, ld, ka, ambient,
			specularExp, ppc->GetVD(), ppc, light);
	}
}

/*
void TM::RenderUpdateIZB(PPC* ppc, FrameBuffer* fb, V3 matColor, V3 ld, float ka,
	float ambientFactor, float specularExp, class Light* light) {

	for (int tri = 0; tri < trisN; tri++) {
		// get vertices and normals
		V3 vs[3]; V3 ns[3]; V3 cs[3];
		for (int vi = 0; vi < 3; vi++) {
			vs[vi] = verts[tris[3 * tri + vi]];
			ns[vi] = normals[tris[3 * tri + vi]];
			cs[vi] = cols[tris[3 * tri + vi]];
		}

		V3 newMatColor = cs[0];

		// project vertices
		V3 pvs[3];
		int needContinue = 0;
		for (int vi = 0; vi < 3; vi++) {
			if (!ppc->Project(vs[vi], pvs[vi])) {
				needContinue = 1;
				break;
			}
		}
		if (needContinue)
			continue;

		V3 ambient = matColor * ambientFactor;

		fb->UpdateIZB2(pvs, ns, cs, newMatColor, ld, ka, ambient,
			specularExp, ppc->GetVD(), ppc, light);
	}
}
*/

// Filled mode render with Phong shading and shadows
void TM::RenderFillWithShadow(PPC* ppc, FrameBuffer* fb, V3 matColor, V3 ld, float ka, 
	float ambientFactor, float specularExp, class Light* light) {

	for (int tri = 0; tri < trisN; tri++) {
		// get vertices and normals
		V3 vs[3]; V3 ns[3]; V3 cs[3];
		for (int vi = 0; vi < 3; vi++) {
			vs[vi] = verts[tris[3 * tri + vi]];
			ns[vi] = normals[tris[3 * tri + vi]];
			cs[vi] = cols[tris[3 * tri + vi]];
		}

		V3 newMatColor = cs[0];

		// project vertices
		V3 pvs[3];
		int needContinue = 0;
		for (int vi = 0; vi < 3; vi++) {
			if (!ppc->Project(vs[vi], pvs[vi])) {
				needContinue = 1;
				break;
			}
		}
		if (needContinue)
			continue;

		float x[3]; x[0] = pvs[0][0]; x[1] = pvs[1][0]; x[2] = pvs[2][0];
		float y[3]; y[0] = pvs[0][1]; y[1] = pvs[1][1]; y[2] = pvs[2][1];
		float z[3]; z[0] = pvs[0][2]; z[1] = pvs[1][2]; z[2] = pvs[2][2];
		V3 ambient = matColor * ambientFactor;

		fb->DrawTriangleWithShadow(x, y, z, ns, newMatColor, ld, ka, ambient, 
			specularExp, ppc->GetVD(), ppc, light);
	}
}

/*
void TM::RenderIrregularShadows(class Light* light, FrameBuffer* fb) {
	for (int tri = 0; tri < trisN; tri++) {
		V3 v0 = verts[tris[3 * tri]];
		V3 v1 = verts[tris[3 * tri + 1]];
		V3 v2 = verts[tris[3 * tri + 2]];
		V3 pv0, pv1, pv2;
		if (!light->Project(v0, pv0))
			continue;
		if (!light->Project(v1, pv1))
			continue;
		if (!light->Project(v2, pv2))
			continue;
		float x[3]; x[0] = pv0[0]; x[1] = pv1[0]; x[2] = pv2[0];
		float y[3]; y[0] = pv0[1]; y[1] = pv1[1]; y[2] = pv2[1];
		float z[3]; z[0] = pv0[2]; z[1] = pv1[2]; z[2] = pv2[2];
		fb->DrawIZBShadow(x, y, z);
	}
}
*/

void TM::RenderIrregularShadows(class Light* light, FrameBuffer* fb) {

	for (int tri = 0; tri < trisN; tri++) {
		V3 vs[3];
		for (int vi = 0; vi < 3; vi++) {
			vs[vi] = verts[tris[3 * tri + vi]];
		}

		// project vertices
		V3 pvs[3];
		int needContinue = 0;
		for (int vi = 0; vi < 3; vi++) {
			if (!light->Project(vs[vi], pvs[vi])) {
				needContinue = 1;
				break;
			}
		}
		if (needContinue)
			continue;

		fb->DrawIZBShadow2(pvs);
	}
}

void TM::RenderFillShadow(PPC* ppc, FrameBuffer* fb, class Light* light) {

	for (int tri = 0; tri < trisN; tri++) {
		V3 vs[3], cs[3], ns[3];
		for (int vi = 0; vi < 3; vi++) {
			vs[vi] = verts[tris[3 * tri + vi]];
			cs[vi] = cols[tris[3 * tri + vi]];
			ns[vi] = normals[tris[3 * tri + vi]];
		}

		// project vertices
		V3 pvs[3];
		int needContinue = 0;
		for (int vi = 0; vi < 3; vi++) {
			if (!ppc->Project(vs[vi], pvs[vi])) {
				needContinue = 1;
				break;
			}
		}
		if (needContinue)
			continue;

		// clip axis aligned bounding box
		AABB aabb(pvs[0]);
		aabb.AddPoint(pvs[1]);
		aabb.AddPoint(pvs[2]);
		if (!aabb.ClipWithFrame(fb->w, fb->h))
			continue;
		int top = (int)(aabb.minv[1] + 0.5f);
		int bottom = (int)(aabb.maxv[1] - 0.5f);
		int left = (int)(aabb.minv[0] + 0.5f);
		int right = (int)(aabb.maxv[0] - 0.5f);

		// setup edge equations
		M33 eeqs;
		eeqs[0].SetAsEdgeEquation(pvs[0], pvs[1], pvs[2]);
		eeqs[1].SetAsEdgeEquation(pvs[1], pvs[2], pvs[0]);
		eeqs[2].SetAsEdgeEquation(pvs[2], pvs[0], pvs[1]);

		// setup screen space interpolation
		M33 ssim; ssim[0] = pvs[0]; ssim[1] = pvs[1]; ssim[2] = pvs[2];
		ssim.SetColumn(2, V3(1.0f, 1.0f, 1.0f)); ssim = ssim.Inverted();
		V3 le1w = ssim * V3(pvs[0][2], pvs[1][2], pvs[2][2]);
		M33 colm; colm[0] = cs[0]; colm[1] = cs[1]; colm[2] = cs[2];
		M33 lecols = (ssim * colm).Transposed();
		M33 nm; nm[0] = ns[0]; nm[1] = ns[1]; nm[2] = ns[2];
		M33 lenormals = (ssim * nm).Transposed();

		for (int v = top; v <= bottom; v++) {
			for (int u = left; u <= right; u++) {
				V3 pixv((float)u, (float)v, 1.0f);
				V3 svs = eeqs * pixv;
				if (svs[0] < 0.0f || svs[1] < 0.0f || svs[2] < 0.0f)
					continue;
				float currz = le1w * pixv;
				if (fb->IsCloserThenSet(currz, u, v)) {

					V3 currCol = lecols * pixv;
					V3 currNormal = (lenormals * pixv).UnitVector();
					//currCol = currCol.Light(V3(1.0f, 0.0f, 0.0f), 0.1f,
						//V3(0.0f, 0.0f, -1.0f), currNormal);

					V3 unproject;
					ppc->Unproject(V3(u, v, currz), unproject);
					V3 smProject;
					light->Project(unproject, smProject);
					if (smProject[0] > 0 && smProject[0] < sm_width && smProject[1] > 0 && smProject[1] < sm_height &&
						fb->sm[(sm_height - 1 - (int)smProject[1]) * sm_width + (int)smProject[0]] > smProject[2] + 0.001f) {
							// if d - d_map < eps, illuminate
							//float bias = max(0.05 * (1.0 - currNormal * light->GetVD()), 0.005);
							fb->Set(u, v, fb->MakeColDarker(currCol.GetColor(), 6.0f));
					} else {
						fb->Set(u, v, (int)currCol.GetColor());
					}
				}
			}
		}
	}

}


V3 TM::GetCenter() {

	V3 ret(0.0f, 0.0f, 0.0f);
	for (int vi = 0; vi < vertsN; vi++)
		ret = ret + verts[vi];
	ret = ret / (float)vertsN;
	return ret;

}

void TM::Rotate(V3 aO, V3 aD, float theta) {

	for (int vi = 0; vi < vertsN; vi++) {
		verts[vi] = verts[vi].RotateThisPoint(aO, aD, theta);
	}

}


void TM::Translate(V3 tv) {

	for (int vi = 0; vi < vertsN; vi++)
		verts[vi] = verts[vi] + tv;

}

void TM::SetCenter(V3 newCenter) {

	V3 center = GetCenter();
	V3 tv = newCenter - center;
	Translate(tv);

}


void TM::Light(V3 matColor, float ka, V3 ld) {

	if (!normals) {
		cerr << "INFO: need normals" << endl;
		return;
	}
	for (int vi = 0; vi < vertsN; vi++) {
		cols[vi] = verts[vi].Light(matColor, ka, ld, normals[vi]);
	}

}


void TM::VisualizeVertexNormals(PPC *ppc, FrameBuffer *fb, float vlength) {

	if (!normals)
		return;
	for (int vi = 0; vi < vertsN; vi++) {
		fb->Render3DSegment(ppc, verts[vi], verts[vi] + normals[vi] * vlength,
			V3(0.0f, 0.0f, 0.0f), V3(1.0f, 0.0f, 0.0f));
	}
	fb->redraw();

}

void TM::SetUnshared(TM *tm) {

	trisN = tm->trisN;
	vertsN = tm->trisN * 3;
	verts = new V3[vertsN];
	tris = new unsigned int[trisN*3];
	if (tm->cols)
		cols = new V3[vertsN];
	if (tm->normals)
		normals = new V3[vertsN];

	for (int ti = 0; ti < tm->trisN; ti++) {
		verts[3 * ti + 0] = tm->verts[tm->tris[ti * 3 + 0]];
		verts[3 * ti + 1] = tm->verts[tm->tris[ti * 3 + 1]];
		verts[3 * ti + 2] = tm->verts[tm->tris[ti * 3 + 2]];
		if (cols) {
			cols[3 * ti + 0] = tm->cols[tm->tris[ti * 3 + 0]];
			cols[3 * ti + 1] = tm->cols[tm->tris[ti * 3 + 1]];
			cols[3 * ti + 2] = tm->cols[tm->tris[ti * 3 + 2]];
		}
		if (normals) {
			normals[3 * ti + 0] = tm->normals[tm->tris[ti * 3 + 0]];
			normals[3 * ti + 1] = tm->normals[tm->tris[ti * 3 + 1]];
			normals[3 * ti + 2] = tm->normals[tm->tris[ti * 3 + 2]];
		}
		tris[3 * ti + 0] = 3 * ti + 0;
		tris[3 * ti + 1] = 3 * ti + 1;
		tris[3 * ti + 2] = 3 * ti + 2;
	}


}

void TM::SetAsCopy(TM *tm) {

	vertsN = tm->vertsN;
	trisN = tm->trisN;
	verts = new V3[vertsN];
	for (int vi = 0; vi < vertsN; vi++)
		verts[vi] = tm->verts[vi];
	tris = new unsigned int[3 * trisN];
	for (int ti = 0; ti < trisN*3; ti++)
		tris[ti] = tm->tris[ti];
	if (tm->cols) {
		cols = new V3[vertsN];
		for (int vi = 0; vi < vertsN; vi++)
			cols[vi] = tm->cols[vi];
	}
	if (tm->normals) {
		normals = new V3[vertsN];
		for (int vi = 0; vi < vertsN; vi++)
			normals[vi] = tm->normals[vi];
	}

}

void TM::Explode(float t) {
	for (int ti = 0; ti < trisN; ti++) {
		unsigned int inds[3];
		inds[0] = tris[3 * ti + 0];
		inds[1] = tris[3 * ti + 1];
		inds[2] = tris[3 * ti + 2];
		// v = (v1-v0) ^ (v2-v0)
		V3 n = (verts[inds[1]] - verts[inds[0]]) ^
			(verts[inds[2]] - verts[inds[0]]);
		n = n.UnitVector();
		verts[inds[0]] = verts[inds[0]] + n*t;
		verts[inds[1]] = verts[inds[1]] + n*t;
		verts[inds[2]] = verts[inds[2]] + n*t;
	}
}

void TM::SetOneTriangle(V3 v0, V3 v1, V3 v2) {

	vertsN = 3;
	trisN = 1;
	verts = new V3[vertsN];
	cols = new V3[vertsN];
	normals = new V3[vertsN];

	verts[0] = v0;
	verts[1] = v1;
	verts[2] = v2;
	cols[0] = V3(0.0f, 1.0f, 0.0f);
	cols[1] = cols[0];
	cols[2] = cols[0];
	V3 n = ((v1 - v0) ^ (v2 - v0)).UnitVector();
	normals[0] =
		normals[1] =
		normals[2] = n;

	tris = new unsigned int[3 * trisN];
	tris[0] = 0;
	tris[1] = 1;
	tris[2] = 2;
}

void TM::SetPlane(V3 cc, float length, float width) {
	vertsN = 4;
	trisN = 2;
	verts = new V3[vertsN];
	cols = new V3[vertsN];
	normals = new V3[vertsN];

	verts[0] = cc + V3(-length / 2, 0.0f, width / 2);
	verts[1] = cc + V3(-length / 2, 0.0f, -width / 2);
	verts[2] = cc + V3(length / 2, 0.0f, -width / 2);
	verts[3] = cc + V3(length / 2, 0.0f, width / 2);
	cols[0] =
		cols[1] =
		cols[2] =
		cols[3] = V3(0.5f, 0.0f, 1.0f);
	V3 n = ((verts[1] - verts[0]) ^ (verts[2] - verts[0])).UnitVector();
	normals[0] =
		normals[1] =
		normals[2] =
		normals[3] = n;

	tris = new unsigned int[3 * trisN];
	tris[0] = 0;
	tris[1] = 1;
	tris[2] = 2;
	tris[3] = 0;
	tris[4] = 2;
	tris[5] = 3;
}

void TM::ReflectVertices(V3 gpCenter, TM ogMesh) {
	for (int i = 0; i < vertsN; i++) {
        float refCenter = ogMesh.verts[i][2] - 2 * (ogMesh.verts[i][2] - gpCenter[2]);
		V3 newV = V3(ogMesh.verts[i][0], ogMesh.verts[i][1], refCenter);
		verts[i] = newV;
	}
}

void TM::ScaleMesh(V3 sv) {
	for (int i = 0; i < vertsN; i++) {
		verts[i] = V3(sv[0]*verts[i][0], sv[1] * verts[i][1], sv[2] * verts[i][2]);
	}
}
