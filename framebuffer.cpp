
#include "framebuffer.h"
#include "math.h"
#include <iostream>
#include "scene.h"
#include "ppc.h"
#include "tm.h"

#include <tiffio.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace std;

const int sm_height = 200;
const int sm_width = 200;

const float pi = atan(1) * 4;

FrameBuffer::FrameBuffer(int u0, int v0, int _w, int _h) : 
	Fl_Gl_Window(u0, v0, _w, _h, 0) {

	w = _w;
	h = _h;
	pix = new unsigned int[w*h];
	panomap = new unsigned int[w*h];
	zb = new float[w*h];
	sm = new float[sm_width*sm_height];
	izb = new sample_ll*[sm_width * sm_height];

	// initalize izb array to NULL
	for (int i = 0; i < sm_width * sm_height; ++i) {
		*(izb + i) = NULL;
	}

}

void FrameBuffer::CopyFB(FrameBuffer* fb) {
	for (int v = 0; v < h; v++) {
		for (int u = 0; u < w; u++) {
			Set(u, v, fb->GetCol(u, v));
		}
	}
}


void FrameBuffer::draw() {

	glDrawPixels(w, h, GL_RGBA, GL_UNSIGNED_BYTE, pix);

}

int FrameBuffer::handle(int event) {

	switch (event)
	{
	case FL_KEYBOARD: {
		KeyboardHandle();
		return 0;
	}
	case FL_MOVE: {
		int u = Fl::event_x();
		int v = Fl::event_y();
		cerr << u << " " << v << "      \r";
		return 0;
	}
	default:
		return 0;
	}
}

void FrameBuffer::KeyboardHandle() {
	int key = Fl::event_key();
	switch (key) {
	case FL_Left: {
		scene->Left();
		break;
	}
	case FL_Right: {
		scene->Right();
		break;
	}
	case FL_Up: {
		scene->Up();
		break;
	}
	case FL_Down: {
		scene->Down();
		break;
	}
	case '\'': {
		scene->Front();
		break;
	}
	case '\"': {
		scene->Back();
		break;
	}
	case '=': {
		scene->ZoomIn();
		break;
	}
	case '-': {
		scene->ZoomOut();
		break;
	}
	case 'e': {
		scene->RotateX();
		break;
	}
	case 'q': {
		scene->RotateXn();
		break;
	}
	case 'w': {
		scene->Tilt();
		break;
	}
	case 's': {
		scene->TiltN();
		break;
	}
	case 'd': {
		scene->Pan();
		break;
	}
	case 'a': {
		scene->PanN();
		break;
	}
	case 'l': {
		scene->LightR();
		break;
	}
	case 'j': {
		scene->LightL();
		break;
	}
	case 'i': {
		scene->LightU();
		break;
	}
	case 'k': {
		scene->LightD();
		break;
	}
	default:
		cerr << "INFO: do not understand keypress" << endl;
		return;
	}

}

void FrameBuffer::SetBGR(unsigned int bgr) {

	for (int uv = 0; uv < w*h; uv++)
		pix[uv] = bgr;

}

void FrameBuffer::SetZB(float zf) {
	for (int uv = 0; uv < w*h; uv++)
		zb[uv] = zf;
}

void FrameBuffer::SetSM(float sf) {
	for (int uv = 0; uv < sm_width * sm_height; uv++)
		sm[uv] = sf;
}

void FrameBuffer::ResetIZB() {
	// initalize izb array to NULL
	for (int i = 0; i < sm_width * sm_height; ++i) {
		*(izb + i) = NULL;
	}
}

void FrameBuffer::SetChecker(unsigned int col0, unsigned int col1, int csize) {

	for (int v = 0; v < h; v++) {
		for (int u = 0; u < w; u++) {
			int cu = u / csize;
			int cv = v / csize;
			if ((cu + cv) % 2)
				Set(u, v, col0);
			else
				Set(u, v, col1);
		}
	}

}

void FrameBuffer::SetShadow(V3 pvs[3]) {

	// clip axis aligned bounding box
	AABB aabb(pvs[0]);
	aabb.AddPoint(pvs[1]);
	aabb.AddPoint(pvs[2]);
	if (aabb.ClipWithFrame(sm_width, sm_height)) {
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

		for (int v = top; v <= bottom; v++) {
			for (int u = left; u <= right; u++) {
				V3 pixv(u, v, 1.0f);
				V3 svs = eeqs * pixv;
				if (svs[0] > 0.0f && svs[1] > 0.0f && svs[2] > 0.0f) {
					float currz = le1w * pixv;
					IsCloserThenSetSM(currz, u, v);
				}
			}
		}
	}
}

void FrameBuffer::DrawAARectangle(V3 tlc, V3 brc, unsigned int col) {

	// entire rectangle off screen
	if (tlc[0] > (float)w)
		return;
	if (brc[0] < 0.0f)
		return;
	if (tlc[1] > (float)h)
		return;
	if (brc[1] < 0.0f)
		return;

	// rectangle partially off screen
	if (tlc[0] < 0.0f)
		tlc[0] = 0.0f;
	if (brc[0] > (float)w)
		brc[0] = (float)w;
	if (tlc[1] < 0.0f)
		tlc[1] = 0.0f;
	if (brc[1] > (float)h)
		brc[1] = (float)h;

	int umin = (int)(tlc[0]+0.5f);
	int umax = (int)(brc[0]-0.5f);
	int vmin = (int)(tlc[1]+0.5f);
	int vmax = (int)(brc[1]-0.5f);
	for (int v = vmin; v <= vmax; v++) {
		for (int u = umin; u <= umax; u++) {
			Set(u, v, col);
		}
	}

}

void FrameBuffer::DrawDisk(V3 center, float r, unsigned int col) {

	int umin = (int)(center[0] - r);
	int umax = (int)(center[0] + r);
	int vmin = (int)(center[1] - r);
	int vmax = (int)(center[1] + r);
	center[2] = 0.0f;
	for (int v = vmin; v <= vmax; v++) {
		for (int u = umin; u <= umax; u++) {
			V3 pixCenter(.5f + (float)u, .5f + (float)v, 0.0f);
			V3 distVector = pixCenter - center;
			if (r*r < distVector * distVector)
				continue;
			SetGuarded(u, v, col);
		}
	}


}

void FrameBuffer::Set(int u, int v, int col) {

	pix[(h - 1 - v)*w + u] = col;

}


int FrameBuffer::GetCol(int u, int v) {

	return pix[(h - 1 - v) * w + u];

}

void FrameBuffer::SetGuarded(int u, int v, int col) {

	if (u < 0 || u > w - 1 || v < 0 || v > h - 1)
		return;
	Set(u, v, col);

}

void FrameBuffer::DrawSegment(V3 p0, V3 p1, unsigned int col) {

	V3 cv;
	cv.SetFromColor(col);
	DrawSegment(p0, p1, cv, cv);

}

void FrameBuffer::DrawSegment(V3 p0, V3 p1, V3 c0, V3 c1) {

	float maxSpan = (fabsf(p0[0] - p1[0]) < fabsf(p0[1] - p1[1])) ?
		fabsf(p0[1] - p1[1]) : fabsf(p0[0] - p1[0]);
	int segsN = (int)maxSpan + 1;
	V3 currPoint = p0;
	V3 currColor = c0;
	V3 stepv = (p1 - p0) / (float)segsN;
	V3 stepcv = (c1 - c0) / (float)segsN;
	int si;
	for (si = 0; 
		si <= segsN; 
		si++, currPoint = currPoint + stepv, currColor = currColor + stepcv) {
		int u = (int)currPoint[0];
		int v = (int)currPoint[1];
		if (u < 0 || u > w - 1 || v < 0 || v > h - 1)
			continue;
		if (IsCloserThenSet(currPoint[2], u, v))
			SetGuarded(u, v, currColor.GetColor());
	}

}

void FrameBuffer::LoadTiff(char* fname) {
	TIFF* in = TIFFOpen(fname, "r");
	if (in == NULL) {
		cerr << fname << " could not be opened" << endl;
		return;
	}

	int width, height;
	TIFFGetField(in, TIFFTAG_IMAGEWIDTH, &width);
	TIFFGetField(in, TIFFTAG_IMAGELENGTH, &height);
	if (w != width || h != height) {
		w = width;
		h = height;
		delete[] pix;
		pix = new unsigned int[w*h];
		size(w, h);
		glFlush();
		glFlush();
	}

	if (TIFFReadRGBAImage(in, w, h, pix, 0) == 0) {
		cerr << "failed to load " << fname << endl;
	}

	TIFFClose(in);
}

void FrameBuffer::SaveAsTiff(char *fname) {

	TIFF* out = TIFFOpen(fname, "w");

	if (out == NULL) {
		cerr << fname << " could not be opened" << endl;
		return;
	}

	TIFFSetField(out, TIFFTAG_IMAGEWIDTH, w);
	TIFFSetField(out, TIFFTAG_IMAGELENGTH, h);
	TIFFSetField(out, TIFFTAG_SAMPLESPERPIXEL, 4);
	TIFFSetField(out, TIFFTAG_BITSPERSAMPLE, 8);
	TIFFSetField(out, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
	TIFFSetField(out, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
	TIFFSetField(out, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);

	for (uint32 row = 0; row < (unsigned int)h; row++) {
		TIFFWriteScanline(out, &pix[(h - row - 1) * w], row);
	}

	TIFFClose(out);
}

void FrameBuffer::Render3DSegment(PPC *ppc, V3 v0, V3 v1, V3 c0, V3 c1) {
	V3 pv0, pv1;
	if (!ppc->Project(v0, pv0))
		return;
	if (!ppc->Project(v1, pv1))
		return;
	DrawSegment(pv0, pv1, c0, c1);
}

void FrameBuffer::Render3DPoint(PPC *ppc, V3 p, V3 c, float psize) {
	V3 pv;
	if (!ppc->Project(p, pv))
		return;
	DrawDisk(pv, psize, c.GetColor());
}

// Draw triangle for Phong Shading / per pixel lighting
// Interpolate vertex normals across triangle and
// use normal at pixel to evaluate lighting equation
void FrameBuffer::DrawTriangle(float x[3], float y[3], float z[3], V3 n[3], V3 matColor, V3 ld,
	float ka, V3 ambient, float specularExp, V3 vd) {
	float a[3], b[3], c[3]; // a, b, c for 3 edge expressions
	a[0] = y[0] - y[1]; b[0] = x[1] - x[0]; c[0] = x[0] * y[1] - x[1] * y[0];
	a[1] = y[1] - y[2]; b[1] = x[2] - x[1]; c[1] = x[1] * y[2] - x[2] * y[1];
	a[2] = y[2] - y[0]; b[2] = x[0] - x[2]; c[2] = x[2] * y[0] - x[0] * y[2];
	float f0 = a[1] * x[0] + b[1] * y[0] + c[1]; // f12(x0,y0)
	float f1 = a[2] * x[1] + b[2] * y[1] + c[2]; // f20(x1,y1)
	float f2 = a[0] * x[2] + b[0] * y[2] + c[0]; // f01(x2,y2)
	// compute screen axes-aligned bounding box for triangle
	float bbox[2][2]; // for each x and y, store the min and max values
	// compute boundary box
	bbox[0][0] = min(min(x[0], x[1]), x[2]);
	bbox[0][1] = max(max(x[0], x[1]), x[2]);
	bbox[1][0] = min(min(y[0], y[1]), y[2]);
	bbox[1][1] = max(max(y[0], y[1]), y[2]);


	int left = (int)(bbox[0][0] + .5), right = (int)(bbox[0][1] - .5);
	int top = (int)(bbox[1][0] + .5), bottom = (int)(bbox[1][1] - .5);

	//ClipBBox(bbox, 0, w, 0, h);
	// entire boundary box off screen
	if (left > w)
		return;
	if (right < 0)
		return;
	if (top > h)
		return;
	if (bottom < 0)
		return;

	// boundary box partially off screen
	if (left < 0)
		left = 0;
	if (right > w)
		right = w;
	if (bottom > h)
		bottom = h;
	if (top < 0)
		top = 0;

	for (int v = top; v <= bottom; v++) {
		for (int u = left; u <= right; u++) {
			float wa = (a[1] * (float)u + b[1] * (float)v + c[1]) / f0;
			float wb = (a[2] * (float)u + b[2] * (float)v + c[2]) / f1;
			float wc = (a[0] * (float)u + b[0] * (float)v + c[0]) / f2;

			if (wa > 0 && wb > 0 && wc > 0) {
				// interpolated normal
				V3 ipN = n[0] * wa + n[1] * wb + n[2] * wc;
				// interpolated z-value
				float ipZ = z[0] * wa + z[1] * wb + z[2] * wc;
				// reflected vector
				V3 rd = ld.Reflect(ipN);
				V3 specular = ld.Specular(vd, rd, specularExp, matColor);
				if (zb[(h - 1 - v) * w + u] < ipZ) {
					zb[(h - 1 - v) * w + u] = ipZ;
					float kd = (ld * -1.0f) * ipN;
					kd = (kd < 0.0f) ? 0.0f : kd;
					V3 pCol = matColor * (ka + (1.0f - ka) * kd) + ambient + specular;
					Set(u, v, (int)pCol.GetColor());
				}
			}
		}
	}
}

void FrameBuffer::RenderFill(float x[3], float y[3], float z[3], V3 n[3], V3 matColor, V3 ld,
	float ka, V3 ambient, float specularExp, V3 vd, PPC* ppc, class Light* light) {

	float a[3], b[3], c[3]; // a, b, c for 3 edge expressions
	a[0] = y[0] - y[1]; b[0] = x[1] - x[0]; c[0] = x[0] * y[1] - x[1] * y[0];
	a[1] = y[1] - y[2]; b[1] = x[2] - x[1]; c[1] = x[1] * y[2] - x[2] * y[1];
	a[2] = y[2] - y[0]; b[2] = x[0] - x[2]; c[2] = x[2] * y[0] - x[0] * y[2];
	float f0 = a[1] * x[0] + b[1] * y[0] + c[1]; // f12(x0,y0)
	float f1 = a[2] * x[1] + b[2] * y[1] + c[2]; // f20(x1,y1)
	float f2 = a[0] * x[2] + b[0] * y[2] + c[0]; // f01(x2,y2)
	// compute screen axes-aligned bounding box for triangle
	float bbox[2][2]; // for each x and y, store the min and max values
	// compute boundary box
	bbox[0][0] = min(min(x[0], x[1]), x[2]);
	bbox[0][1] = max(max(x[0], x[1]), x[2]);
	bbox[1][0] = min(min(y[0], y[1]), y[2]);
	bbox[1][1] = max(max(y[0], y[1]), y[2]);

	int left = (int)(bbox[0][0] + .5), right = (int)(bbox[0][1] - .5);
	int top = (int)(bbox[1][0] + .5), bottom = (int)(bbox[1][1] - .5);

	//ClipBBox(bbox, 0, w, 0, h);
	// entire boundary box off screen
	if (left > w)
		return;
	if (right < 0)
		return;
	if (top > h)
		return;
	if (bottom < 0)
		return;

	// boundary box partially off screen
	if (left < 0)
		left = 0;
	if (right > w)
		right = w;
	if (bottom > h)
		bottom = h;
	if (top < 0)
		top = 0;

	for (int v = top; v < bottom; v++) {
		for (int u = left; u < right; u++) {
			float wa = (a[1] * (float)u + b[1] * (float)v + c[1]) / f0;
			float wb = (a[2] * (float)u + b[2] * (float)v + c[2]) / f1;
			float wc = (a[0] * (float)u + b[0] * (float)v + c[0]) / f2;

			if (wa > 0 && wb > 0 && wc > 0) {
				// interpolated normal
				V3 ipN = n[0] * wa + n[1] * wb + n[2] * wc;
				// interpolated z-value
				float ipZ = z[0] * wa + z[1] * wb + z[2] * wc;
				// reflected vector
				V3 rd = ld.Reflect(ipN);
				V3 specular = ld.Specular(vd, rd, specularExp, matColor);

				if (zb[(h - 1 - v) * w + u] < ipZ) {
					// update z-buffer
					zb[(h - 1 - v) * w + u] = ipZ;

					// lighting equation
					float kd = (ld * -1.0f) * ipN;
					kd = (kd < 0.0f) ? 0.0f : kd;
					V3 pCol = matColor * (ka + (1.0f - ka) * kd) + ambient + specular;

					// set col
					Set(u, v, (int)pCol.GetColor());
				}
			}
		}
	}
}

void FrameBuffer::UpdateIZB(PPC* ppc, Light* light) {
	for (int v = 0; v < h; v++) {
		for (int u = 0; u < w; u++) {
			// unproject p to 3D at P
			V3 unproject;
			ppc->Unproject(V3((float)u, (float)v, zb[(h - 1 - v) * w + u]), unproject);

			// project P to 2D point q on SM using light
			V3 smProject;
			light->Project(unproject, smProject);

			// if the volume of space the eye sees is seen by the light view
			if (smProject[0] > 0 && smProject[0] < sm_width && smProject[1] > 0 && smProject[1] < sm_height) {
				// create new sample
				sample_ll* new_sample = new sample_ll;
				new_sample->u = smProject[0];
				new_sample->v = smProject[1];
				new_sample->z = smProject[2];
				new_sample->u0 = u;
				new_sample->v0 = v;
				new_sample->next = izb[((sm_height - 1 - (int)smProject[1]) * sm_width + (int)smProject[0])];
				izb[((sm_height - 1 - (int)smProject[1]) * sm_width + (int)smProject[0])] = new_sample;
			}
		}
	}
}

void FrameBuffer::UpdateIZB2(V3 pvs[3], V3 ns[3], V3 cs[3], V3 matColor, V3 ld,
	float ka, V3 ambient, float specularExp, V3 vd, PPC* ppc, class Light* light) {

	// clip axis aligned bounding box
	AABB aabb(pvs[0]);
	aabb.AddPoint(pvs[1]);
	aabb.AddPoint(pvs[2]);
	if (aabb.ClipWithFrame(w, h)) {
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
				V3 pixv(.5f + (float)u, .5f + (float)v, 1.0f);
				V3 svs = eeqs * pixv;
				if (svs[0] < 0.0f || svs[1] < 0.0f || svs[2] < 0.0f)
					continue;
				float currz = le1w * pixv;
				if (IsCloserThenSet(currz, u, v)) {
					V3 currCol = lecols * pixv;
					V3 currNormal = (lenormals * pixv).UnitVector();
					currCol = currCol.Light(V3(1.0f, 1.0f, 1.0f), 0.1f,
						V3(0.0f, 0.0f, -1.0f), currNormal);

					// unproject p to 3D at P
					V3 unproject;
					ppc->Unproject(V3((float)u, (float)v, currz), unproject);

					// project P to 2D point q on SM using light
					V3 smProject;
					light->Project(unproject, smProject);

					// if the volume of space the eye sees is seen by the light view
					if (smProject[0] > 0 && smProject[0] < sm_width && smProject[1] > 0 && smProject[1] < sm_height) {
						// create new sample
						sample_ll* new_sample = new sample_ll;
						new_sample->u = smProject[0];
						new_sample->v = smProject[1];
						new_sample->z = smProject[2];
						new_sample->u0 = u;
						new_sample->v0 = v;
						new_sample->next = izb[((sm_height - 1 - (int)smProject[1]) * sm_width + (int)smProject[0])];
						izb[((sm_height - 1 - (int)smProject[1]) * sm_width + (int)smProject[0])] = new_sample;
					}
					Set(u, v, currCol.GetColor());
				}

			}
		}
	}

}

void FrameBuffer::DrawIZBShadow(float x[3], float y[3], float z[3]) {
	float a[3], b[3], c[3]; // a, b, c for 3 edge expressions
	a[0] = y[0] - y[1]; b[0] = x[1] - x[0]; c[0] = x[0] * y[1] - x[1] * y[0];
	a[1] = y[1] - y[2]; b[1] = x[2] - x[1]; c[1] = x[1] * y[2] - x[2] * y[1];
	a[2] = y[2] - y[0]; b[2] = x[0] - x[2]; c[2] = x[2] * y[0] - x[0] * y[2];
	float f0 = a[1] * x[0] + b[1] * y[0] + c[1]; // f12(x0,y0)
	float f1 = a[2] * x[1] + b[2] * y[1] + c[2]; // f20(x1,y1)
	float f2 = a[0] * x[2] + b[0] * y[2] + c[0]; // f01(x2,y2)
	// compute screen axes-aligned bounding box for triangle
	float bbox[2][2]; // for each x and y, store the min and max values
	// compute boundary box
	bbox[0][0] = min(min(x[0], x[1]), x[2]);
	bbox[0][1] = max(max(x[0], x[1]), x[2]);
	bbox[1][0] = min(min(y[0], y[1]), y[2]);
	bbox[1][1] = max(max(y[0], y[1]), y[2]);

	int left = (int)(bbox[0][0] + .5), right = (int)(bbox[0][1] - .5);
	int top = (int)(bbox[1][0] + .5), bottom = (int)(bbox[1][1] - .5);

	//ClipBBox(bbox, 0, w, 0, h);
	// entire boundary box off screen
	if (left > sm_width)
		return;
	if (right < 0)
		return;
	if (top > sm_height)
		return;
	if (bottom < 0)
		return;

	// boundary box partially off screen
	if (left < 0)
		left = 0;
	if (right > sm_width)
		right = sm_width;
	if (bottom > sm_height)
		bottom = sm_height;
	if (top < 0)
		top = 0;

	for (int v = top; v < bottom; v++) {
		for (int u = left; u < right; u++) {

				// grab all samples at that texel
				sample_ll* texel = *(izb + (sm_height - 1 - (int) v) * sm_width + (int) u);

				while (texel != NULL) {
					float wa = (a[1] * texel->u + b[1] * texel->v + c[1]) / f0;
					float wb = (a[2] * texel->u + b[2] * texel->v + c[2]) / f1;
					float wc = (a[0] * texel->u + b[0] * texel->v + c[0]) / f2;
					float ipZ = z[0] * wa + z[1] * wb + z[2] * wc;

					// if sample is in triangle and interpolated z is closer than sample z
					if (wa > 0 && wb > 0 && wc > 0 && texel->z + 0.01f < ipZ) {
						unsigned int prevCol = GetCol(texel->u0, texel->v0);
						Set(texel->u0, texel->v0, MakeColDarker(prevCol, 2.0f));
					}
					texel = texel->next;
				}
		}
	}
}

void FrameBuffer::DrawIZBShadow2(V3 pvs[3]) {
	// clip axis aligned bounding box
	AABB aabb(pvs[0]);
	aabb.AddPoint(pvs[1]);
	aabb.AddPoint(pvs[2]);
	if (aabb.ClipWithFrame(sm_width, sm_height)) {
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

		for (int v = top; v <= bottom; v++) {
			for (int u = left; u <= right; u++) {
				// grab all samples at that texel
				sample_ll* texel = *(izb + (sm_height - 1 - v) * sm_width + u);

				while (texel != NULL) {
					V3 pixv(texel->u, texel->v, 1.0f);
					V3 svs = eeqs * pixv;
					float currz = le1w * pixv;
					// if sample is in triangle and interpolated z is closer than sample z
					if (svs[0] > 0.0f && svs[1] > 0.0f && svs[2] > 0.0f && texel->z + 0.01f < currz) {
						unsigned int prevCol = GetCol(texel->u0, texel->v0);
						Set(texel->u0, texel->v0, MakeColDarker(prevCol, 3.0f));
						//Set(texel->u0, texel->v0, V3(1.0f, 0.0f, 0.0f).GetColor());
					}
					texel = texel->next;
				}
			}
		}
	}
}

// Shadows
void FrameBuffer::DrawTriangleWithShadow(float x[3], float y[3], float z[3], V3 n[3], V3 matColor, V3 ld,
	float ka, V3 ambient, float specularExp, V3 vd, PPC* ppc, class Light* light) {
	float a[3], b[3], c[3]; // a, b, c for 3 edge expressions
	a[0] = y[0] - y[1]; b[0] = x[1] - x[0]; c[0] = x[0] * y[1] - x[1] * y[0];
	a[1] = y[1] - y[2]; b[1] = x[2] - x[1]; c[1] = x[1] * y[2] - x[2] * y[1];
	a[2] = y[2] - y[0]; b[2] = x[0] - x[2]; c[2] = x[2] * y[0] - x[0] * y[2];
	float f0 = a[1] * x[0] + b[1] * y[0] + c[1]; // f12(x0,y0)
	float f1 = a[2] * x[1] + b[2] * y[1] + c[2]; // f20(x1,y1)
	float f2 = a[0] * x[2] + b[0] * y[2] + c[0]; // f01(x2,y2)
	// compute screen axes-aligned bounding box for triangle
	float bbox[2][2]; // for each x and y, store the min and max values
	// compute boundary box
	bbox[0][0] = min(min(x[0], x[1]), x[2]);
	bbox[0][1] = max(max(x[0], x[1]), x[2]);
	bbox[1][0] = min(min(y[0], y[1]), y[2]);
	bbox[1][1] = max(max(y[0], y[1]), y[2]);


	int left = (int)(bbox[0][0] - .5), right = (int)(bbox[0][1] + .5);
	int top = (int)(bbox[1][0] - .5), bottom = (int)(bbox[1][1] + .5);

	//ClipBBox(bbox, 0, w, 0, h);
	// entire boundary box off screen
	if (left > w)
		return;
	if (right < 0)
		return;
	if (top > h)
		return;
	if (bottom < 0)
		return;

	// boundary box partially off screen
	if (left < 0)
		left = 0;
	if (right > w)
		right = w;
	if (bottom > h)
		bottom = h;
	if (top < 0)
		top = 0;

	for (int v = top; v < bottom; v++) {
		for (int u = left; u < right; u++) {
			float wa = (a[1] * (float)u + b[1] * (float)v + c[1]) / f0;
			float wb = (a[2] * (float)u + b[2] * (float)v + c[2]) / f1;
			float wc = (a[0] * (float)u + b[0] * (float)v + c[0]) / f2;

			if (wa > 0 && wb > 0 && wc > 0) {
				// interpolated normal
				V3 ipN = n[0] * wa + n[1] * wb + n[2] * wc;
				// interpolated z-value
				float ipZ = z[0] * wa + z[1] * wb + z[2] * wc;
				// reflected vector
				V3 rd = ld.Reflect(ipN);
				V3 specular = ld.Specular(vd, rd, specularExp, matColor);
				if (zb[(h - 1 - v) * w + u] < ipZ) {
					zb[(h - 1 - v) * w + u] = ipZ;
					float kd = (ld * -1.0f) * ipN;
					kd = (kd < 0.0f) ? 0.0f : kd;
					V3 pCol = matColor * (ka + (1.0f - ka) * kd) + ambient + specular;
					V3 unproject;
					ppc->Unproject(V3((float)u, (float)v, ipZ), unproject);
					V3 smProject;
					light->Project(unproject, smProject);
					if (smProject[0] > 0 && smProject[0] < sm_width && smProject[1] > 0 && smProject[1] < sm_height) {
						// if d - d_map < eps, illuminate
						float bias = max(0.05 * (1.0 - ipN * ld), 0.005);
						//cout << (sm[(sm_height - 1 - (int)smProject[1]) * sm_width + (int)smProject[0]]) << "   " << smProject[2] << endl;
						if (sm[(sm_height - 1 - (int)smProject[1]) * sm_width + (int)smProject[0]] > smProject[2] + bias) {
							//Set(u, v, MakeColDarker(pCol.GetColor(), 10.0f));
							Set(u, v, (int)V3(0.0f,0.0f,0.0f).GetColor());
						}
						else {
							Set(u, v, (int)pCol.GetColor());
						}
					}
					else {
						Set(u, v, (int)pCol.GetColor());
					}
				}
			}
		}
	}
}

int FrameBuffer::IsCloserThenSet(float currz, int u, int v) {
	float zbz = zb[(h - 1 - v)*w + u];
	if (zbz > currz)
		return 0;
	zb[(h - 1 - v)*w + u] = currz;
	return 1;
}

int FrameBuffer::IsCloserThenSetSM(float currz, int u, int v) {
	float zbz = sm[(sm_height - 1 - v)*sm_width + u];
	if (zbz > currz)
		return 0;
	sm[(sm_height - 1 - v)*sm_width + u] = currz;
	return 1;
}

void FrameBuffer::ViewShadowMap(float* some_sm) {
	for (int v = 0; v < sm_height; v++) {
		for (int u = 0; u < sm_width; u++) {
			float col = some_sm[(sm_height - 1 - v) * sm_width + u];
			for (int v2 = v * 2; v2 < v * 2 + 2; v2++) {
				for (int u2 = u * 2; u2 < u * 2 + 2; u2++) {
					Set(u2, v2, (int) V3(col / 5.0f, col / 5.0f, col / 5.0f).GetColor());
				}
			}
		}
	}
}

void FrameBuffer::PrintIZB() {
	for (int v = 0; v < sm_height; v++) {
		for (int u = 0; u < sm_width; u++) {
			sample_ll* temp = *(izb + (sm_height - 1 - v) * sm_width + u);

			// linked list number
			cout << u << ", " << v << "-->\t";

			// pjrint the linked list
			while (temp != NULL) {
				cout << temp->u << " " << temp->v << "   ";
				temp = temp->next;
			}
			cout << '\n';
		}
	}
}

void FrameBuffer::ViewIZB(sample_ll** some_izb) {
	for (int v = 0; v < sm_height*3; v+=3) {
		for (int u = 0; u < sm_width*3; u+=3) {
			Set(u, v, (int)V3(1.0f, 0.0f, 0.0f).GetColor());
		}
	}

	for (int v = 0; v < sm_height; v++) {
		for (int u = 0; u < sm_width; u++) {
			sample_ll* temp = *(some_izb + (sm_height - 1 - v) * sm_width + u);

			while (temp != NULL) {
				Set(temp->u * 3, temp->v * 3, (int) V3(temp->z / 5.0f, temp->z / 5.0f, temp->z / 5.0f).GetColor());
				temp = temp->next;
			}
		}
	}
}

unsigned int FrameBuffer::MakeColDarker(unsigned int col, float shade) {
	V3 newCol;
	newCol = newCol.GetVectorColor(col);
	newCol = V3(newCol[0]/shade, newCol[1]/shade, newCol[2]/shade);
	return (int) newCol.GetColor();
}

void FrameBuffer::SetTextureImage(char* fname) {
	int width, height, channels;
	unsigned char* img = stbi_load(fname, &width, &height, &channels, 0);
	if (img == NULL) {
		printf("Error in loading the image\n");
		exit(1);
		printf("Loaded image with a width of %dpx, a height of %dpx and %d channels\n", width, height, channels);
	}
	w = width;
	h = height;
	//printf("Loaded image with a width of %dpx, a height of %dpx and %d channels\n", width, height, channels);
	for (int v = 0; v < h; v++) {
		for (int u = 0; u < w; u++) {
			unsigned bytePerPixel = channels;
			unsigned char* pixelOffset = img + (u + width * v) * bytePerPixel;
			unsigned char r = pixelOffset[0];
			unsigned char g = pixelOffset[1];
			unsigned char b = pixelOffset[2];
			unsigned int col = 0xFF000000 + (b * 256 * 256) + (g * 256) + r;
			Set(u, v, (int)col);
		}
	}
}

void FrameBuffer::GetSpherical(PPC* ppc, FrameBuffer* panorama) {
	for (int v = 0; v < h; v++) {
		for (int u = 0; u < w; u++) {
			V3 ontoSphere = ppc->GetFBIntersection(u, v).UnitVector();
			float theta = (pi + atan2(ontoSphere[1], ontoSphere[0])) / (2 * pi);
			float phi = ((pi / 2) - atan2(ontoSphere[2], sqrt(ontoSphere[0] * ontoSphere[0] + ontoSphere[1] * ontoSphere[1]))) / pi;
			int currCol = panorama->Get((int)panorama->w - ((theta * panorama->w) - 1.0f), (int)((phi * panorama->h) - 1.0f));
			Set(u, v, currCol);
			panorama->SetPanoMap((int)panorama->w - ((theta * panorama->w) - 1.0f), (int)((phi * panorama->h) - 1.0f));
		}
	}
}

void FrameBuffer::GetSpherical(PPC* ppc, FrameBuffer* panorama, V3 gpCenter, class Light* light) {
	for (int v = 0; v < h; v++) {
		for (int u = 0; u < w; u++) {
			// does camera ray intersect ground
			V3 ray = ppc->GetFBIntersection(u, v).UnitVector();
			V3 normal = V3(0.0f, 0.0f, 1.0f);
			float t = (gpCenter - ppc->C) * normal / (ray * normal);

			int inShadow = 0;

			// if camera ray intersects ground, color plane and render z buffer
			if (t >= 0.0f) {
				V3 P = ppc->C + (ray * t);
				ray = P.UnitVector();
				V3 projectedP;
				ppc->Project(P, projectedP);
				if (IsCloserThenSet(projectedP[2], u, v)) {
					V3 smProject;
					light->Project(P, smProject);
					if (smProject[0] > 0 && smProject[0] < sm_width && smProject[1] > 0 && smProject[1] < sm_height) {
						//cout << sm[(sm_height - 1 - (int)smProject[1]) * sm_width + (int)smProject[0]] << "||| " << smProject[2] << endl;
						if (sm[(sm_height - 1 - (int)smProject[1]) * sm_width + (int)smProject[0]] > smProject[2]) {
							inShadow = 1;
						}
					}
				}
			}
			// get panorama lookup
			V3 ontoSphere = ray;
			float theta = (pi + atan2(ontoSphere[1], ontoSphere[0])) / (2 * pi);
			float phi = ((pi / 2) - atan2(ontoSphere[2], sqrt(ontoSphere[0] * ontoSphere[0] + ontoSphere[1] * ontoSphere[1]))) / pi;
			int currCol = panorama->Get((int)panorama->w - ((theta * panorama->w) - 1.0f), (int)((phi * panorama->h) - 1.0f));
			if (inShadow) {
				Set(u, v, MakeColDarker(currCol, 2.0f));
			}
			else {
				Set(u, v, currCol);
			}
			panorama->SetPanoMap((int)panorama->w - ((theta * panorama->w)-1.0f), (int)((phi * panorama->h)-1.0f));
		}
	}
}

int FrameBuffer::LookupSpherical(PPC* ppc, FrameBuffer* panorama, V3 ray) {
	// does camera ray intersect ground
	V3 normal = V3(0.0f, 0.0f, 1.0f);
	float t = (V3(0.0f, 0.0f, -8.0f) - ppc->C) * normal / (ray * normal);

	// if camera ray intersects ground, color plane and render z buffer
	if (t >= 0.0f) {
		V3 P = ppc->C + (ray * t);
		ray = P.UnitVector();
		V3 projectedP;
		ppc->Project(P, projectedP);
	}
	// get panorama lookup
	V3 ontoSphere = ray;
	float theta = (pi + atan2(ontoSphere[1], ontoSphere[0])) / (2 * pi);
	float phi = ((pi / 2) - atan2(ontoSphere[2], sqrt(ontoSphere[0] * ontoSphere[0] + ontoSphere[1] * ontoSphere[1]))) / pi;
	int currCol = panorama->Get((int)panorama->w - ((theta * panorama->w) - 1.0f), (int)((phi * panorama->h) - 1.0f));
	//Set(u, v, currCol);
	return currCol;
}

unsigned int FrameBuffer::Get(int u, int v) {
	return pix[(h - 1 - v) * w + u];
}

void FrameBuffer::SetPanoMap(int u, int v) {

	panomap[(h - 1 - v) * w + u] = 1;

}

void FrameBuffer::RenderPanoMap() {
	for (int v = 0; v < h; v++) {
		for (int u = 0; u < w; u++) {
			if (panomap[(h - 1 - v) * w + u] == 1) {
				V3 myCol;
				myCol = myCol.GetVectorColor(GetCol(u, v));
				myCol = V3(myCol[0], myCol[1]/2.0f, myCol[2]/2.0); // make it red
				Set(u, v, myCol.GetColor());
			}
		}
	}
}

void FrameBuffer::ResetPanoMap() {
	for (int uv = 0; uv < w * h; uv++)
		panomap[uv] = 0;
}
