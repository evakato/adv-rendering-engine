#include "scene.h"

#include "V3.h"
#include "M33.h"
#include "tm.h"

Scene *scene;

using namespace std;

#include <iostream>
#include <fstream>


int u0 = 16;
int v0 = 40;
int h = 400;
int w = 600;

Scene::Scene() {

    mode = 0;

	gui = new GUI();
	gui->show();

    int light_w = 200;
    int light_h = 200;
	fb = new FrameBuffer(u0, v0, w, h);
	fb->position(u0, v0);
	fb->label("first person");
	fb->show();
	fb->SetBGR(0xFF0000FF);
	fb->redraw();

    /*
	fb3 = new FrameBuffer(u0, v0, 1400, 700);
	fb3->position(u0+fb->w+u0, v0);
	fb3->label("3rd person");
	fb3->show();
	fb3->SetBGR(0xFFFF0000);
	fb3->redraw();
    */

    fbs = new FrameBuffer(u0, v0, 400, 400);
    fbs->position(u0 + fb->w + u0, v0 + fb->h + v0);
    fbs->label("Shadow map");
    fbs->show();
    fbs->redraw();

	float hfov = 60.0f;
	ppc = new PPC(hfov, w, h);
	ppc3 = new PPC(hfov, w, h);
    // airport
	//ppc->C = V3(0.0f, 0.0f, 0.0f);

	tmsN = 2;
	tms = new TM[tmsN];
	//tms[0].SetOneTriangle(V3(-50.0f, 0.0f, 0.0f), V3(0.0f, 50.0f, 0.0f), V3(50.0f, 0.0f, 0.0f));

    // set ground plane and cube for pano
    // for airport
	//V3 gpCenter = V3(0.0f, 0.0f, -1.5f);
    // water
    V3 gpCenter = V3(0.0f, 0.0f, -8.0f);
    tms[0].SetPlane(gpCenter, 10.0f, 10.0f);
	tms[1].SetFullCube(gpCenter, 1.5f);
    /*
    tms[1].LoadBin("geometry/teapot1k.bin");
    tms[1].ScaleMesh(V3(0.05f, 0.05f, 0.05f));
	tms[1].Rotate(V3(0,0,0), V3(1,0,0), 90.0f);
	tms[1].Rotate(V3(0,0,0), V3(0,0,1), 150.0f);
    tms[2].LoadBin("geometry/teapot1k.bin");
    */
    tms[1].SetCenter(gpCenter);
    //tms[2].SetFullCube(gpCenter, 1.5f);

	light = new Light(hfov, light_w, light_h);
	//light->C = V3(100.0f, 100.0f, -50.0f);
	light->C = V3(100.0f, 120.0f, -200.0f);
	ambientFactor = 0.3f;

	light->PositionAndOrient(light->C, tms[1].GetCenter(), V3(0.0f, 1.0f, 0.0f));
	ppc3->PositionAndOrient(ppc3->C, tms[0].GetCenter(), V3(0.0f, 1.0f, 0.0f));
	//ppc->PositionAndOrient(ppc->C, tms[0].GetCenter(), V3(0.0f, 1.0f, 0.0f));
	//ppc->PositionAndOrient(ppc->C, V3(0.0f, 10.0f, -1.0f), V3(0.0f, 1.0f, 0.0f));
	gui->uiw->position(u0, v0 + fb->h + v0);


}

void Scene::RenderLSM(FrameBuffer* rfb) {
    rfb->SetSM(0.0f);
    // only need to render the cube in shadow map
    tms[1].RenderShadowMap(light, rfb);
    /*
    for (int tmi = 0; tmi < tmsN; tmi++) {
        tms[tmi].RenderShadowMap(light, rfb);
    }
    */
}

void Scene::Render(PPC *rppc, FrameBuffer *rfb) {
	V3 matColor(1.0f, 1.0f, 1.0f);
	float ka = 0.1f;
	float specularExp = 0.0f;
	rfb->SetBGR(0xFFFFFFFF);
	rfb->SetZB(0.00f);
    ppc->Zoom(4.0f);

    // normal shadow mapping
	//for (int tmi = 0; tmi < tmsN; tmi++) {
 //       tms[tmi].RenderFillShadow(rppc, rfb, light);
	//}
    // irregular z buffering
	for (int tmi = 0; tmi < tmsN; tmi++) {
	    tms[tmi].RenderUpdateIZB(rppc, rfb, matColor, light->GetVD(), ka, ambientFactor, specularExp, light);
	}
    rfb->UpdateIZB(rppc, light);
    for (int tmi = 0; tmi < tmsN; tmi++) {
        tms[tmi].RenderIrregularShadows(light, rfb);
    }
	rfb->redraw();
	Fl::check();
}

void Scene::Render() {
    if (mode == 1) {
        RenderIZBScene();
    } else if (mode == 2) {
        RenderSimpleLightScene();
    } else if (mode == 3) {
        RenderJustPanoramaLoop();
    } else if (mode == 4) {
        RenderGroundPlaneShadows();
    } else if (mode == 5) {
        ReflectionDemoLoop();
    }
    return;

    {
        fb->ResetIZB();
        fb3->ResetIZB();

        RenderLSM(fb);
        RenderLSM(fb3);

        // first person
        Render(ppc, fb);
        // third person
        Render(ppc3, fb3);

        fbs->ViewShadowMap(fb->sm);
        fbs->redraw();

        // ppc1
        fb3->Render3DSegment(ppc3, ppc->C, ppc->C + ppc->GetVD() * 100.0f,
            V3(1.0f, 0.0f, 1.0f), V3(1.0f, 0.0f, 0.0f));
        fb3->Render3DPoint(ppc3, ppc->C, V3(1.0f, 0.0f, 0.0f), 7.0f);

        // light
        fb3->Render3DSegment(ppc3, light->C, light->C + light->GetVD() * 100.0f, V3(1.0f, 1.0f, 0.0f),
            V3(1.0f, 1.0f, 0.0f));
        fb3->Render3DPoint(ppc3, light->C, V3(1.0f, 1.0f, 0.0f), 7.0f);

        fb3->redraw();
        //fb->PrintIZB();
        fbizb->SetBGR(0xFFFFFFFF);
        fbizb->ViewIZB(fb3->izb);
        fbizb->redraw();
        Fl::check();
    }


}

// water reflection animationnn
void Scene::RenderWaterReflectionDemo() {
    int fN = 150;
    for (int fi = 0; fi < fN; fi++) {
        fb->SetZB(0.0f);
        fb->GetSpherical(ppc, fb3, tms[0].GetCenter(), light);

        // render virtual box
        tms[1].RenderFillWithReflection(ppc, fb, fb3);

        // render reflection on water
		fb->SetZB(0.0f);
		tms[1].RenderZBOnly(ppc, fb, tms[0].GetCenter());

        FrameBuffer* currWater = new FrameBuffer(u0, v0, w, h);
        currWater->CopyFB(fb);
		tms[2].ReflectVertices(tms[0].GetCenter(), tms[1]);
        tms[2].RenderFillInWater(ppc, fb, currWater, tms[0].GetCenter());

        fb->redraw();
        Fl::check();

        // update box pos
        tms[1].SetCenter(V3(tms[1].GetCenter()[0], tms[1].GetCenter()[1], tms[1].GetCenter()[2] + 0.02f));
    }
}


void Scene::RenderReflectionDemoCube() {


    // change ppc C for water
    ppc->C = V3(0.0f, 0.0f, -5.0f);
    ppc->Tilt(90.0f);
    ppc->Pan(150.0f);

    fb3 = new FrameBuffer(u0, v0, 1000, 500);
    fb3->SetBGR(0xFFFFFFFF);
    fb3->ResetPanoMap();
    fb3->SetTextureImage("img/water.jpg");
    fb->SetBGR(0xFFFFFFFF);

    tmsN = 3;
    tms = new TM[tmsN];
    V3 gpCenter = V3(0.0f, 0.0f, -8.0f);
    tms[0].SetPlane(gpCenter, 10.0f, 10.0f);
    tms[1].SetFullCube(gpCenter, 1.5f);
    tms[1].SetCenter(V3(ppc->GetVD()[0]*7, ppc->GetVD()[1]*7, tms[0].GetCenter()[2]+1.5f));

    // initially set the reflection mesh exactly the same as the real one
    tms[2].SetFullCube(tms[1].GetCenter(), 1.5f);

    ReflectionDemoLoop();
}



void Scene::RenderReflectionDemo() {

    // change ppc C for water
	ppc->C = V3(0.0f, 0.0f, -5.0f);
    ppc->Tilt(90.0f);
    ppc->Pan(150.0f);

    fb3 = new FrameBuffer(u0, v0, 1000, 500);
    fb3->SetBGR(0xFFFFFFFF);
	fb3->ResetPanoMap();
    fb3->SetTextureImage("img/water.jpg");
	fb->SetBGR(0xFFFFFFFF);

    tmsN = 3;
	tms = new TM[tmsN];
    V3 gpCenter = V3(0.0f, 0.0f, -8.0f);
    tms[0].SetPlane(gpCenter, 10.0f, 10.0f);
	//tms[1].SetFullCube(gpCenter, 1.5f);
    tms[1].LoadBin("geometry/teapot1k.bin");
    tms[1].ScaleMesh(V3(0.05f, 0.05f, 0.05f));
	tms[1].Rotate(V3(0,0,0), V3(1,0,0), 90.0f);
	tms[1].Rotate(V3(0,0,0), V3(0,0,1), 150.0f);
    tms[1].SetCenter(gpCenter);

    // initially set the reflection mesh exactly the same as the real one
    //tms[2].SetFullCube(tms[1].GetCenter(), 1.5f);
    tms[2].LoadBin("geometry/teapot1k.bin");

	// box
	//tms[1].SetCenter(V3(ppc->GetVD()[0]*7, ppc->GetVD()[1]*7, tms[0].GetCenter()[2]-0.5f));
	// teapot
	tms[1].SetCenter(V3(ppc->GetVD()[0]*10, ppc->GetVD()[1]*10, tms[0].GetCenter()[2]+2.0f));
    ReflectionDemoLoop();
}

void Scene::ReflectionDemoLoop() {
    // cube
    tms[1].SetCenter(V3(ppc->GetVD()[0]*7, ppc->GetVD()[1]*7, tms[0].GetCenter()[2]+1.5f));

	fb->SetZB(0.0f);
	fb->GetSpherical(ppc, fb3, tms[0].GetCenter(), light);

	// render virtual box
	tms[1].RenderFillWithReflection(ppc, fb, fb3);

	// render reflection on water
	fb->SetZB(0.0f);
	tms[1].RenderZBOnly(ppc, fb, tms[0].GetCenter());

	FrameBuffer* currWater = new FrameBuffer(u0, v0, w, h);
	currWater->CopyFB(fb);
	tms[2].ReflectVertices(tms[0].GetCenter(), tms[1]);
	tms[2].RenderFillInWater(ppc, fb, currWater, tms[0].GetCenter());

	fb->redraw();
	Fl::check();
}

void Scene::RenderGroundPlaneShadows() {
    ppc->Tilt(90.0f);
    ppc->Pan(-150.0f);

    // geometry for airport
    V3 gpCenter = V3(0.0f, 0.0f, -1.5f);
    tms[0].SetPlane(gpCenter, 10.0f, 10.0f);
    tms[1].SetFullCube(gpCenter, 1.5f);

    // reset everything
    fb3 = new FrameBuffer(u0, v0, 1400, 700);
    fb3->SetBGR(0xFFFFFFFF);
    fb3->SetTextureImage("img/pano.jpeg");
    fb->SetBGR(0xFFFFFFFF);
	fb->SetZB(0.0f);
    fb->ResetIZB();

	// just pano and ground plane and box
	tms[1].SetCenter(V3(ppc->GetVD()[0]*10, ppc->GetVD()[1]*10, tms[0].GetCenter()[2]+0.6f));
	light->C = V3(tms[1].GetCenter()[0]+5.0f, tms[1].GetCenter()[1]+5.0f, tms[0].GetCenter()[2]+4.0f);
	light->PositionAndOrient(light->C, tms[1].GetCenter(), V3(0.0f, 1.0f, 0.0f));

	//int fN = 120;
	//for (int fi = 0; fi < fN; fi++) {

	// set and view shadow map
	fb->SetSM(0.0f);
	tms[1].RenderShadowMap(light, fb);
	fbs->ViewShadowMap(fb->sm);
	fbs->redraw();

    // render env map
	fb->GetSpherical(ppc, fb3, tms[0].GetCenter(), light);

    V3 matColor(0.7f, 1.0f, 0.0f);
    float ka = 0.03f;
    float specularExp = 0.0f;

    // irregular z buffering
    for (int tmi = 0; tmi < tmsN; tmi++) {
        tms[tmi].RenderUpdateIZB(ppc, fb, matColor, light->GetVD(), ka, ambientFactor, specularExp, light);
    }
    fb->UpdateIZB(ppc, light);
    for (int tmi = 0; tmi < tmsN; tmi++) {
        tms[tmi].RenderIrregularShadows(light, fb);
    }

	//tms[1].RenderFilled(ppc, fb);
	fb->redraw();
	Fl::check();
    return;

    // go crazy with the animations

    // 
	//light->TranslateUD(0.1f);
	//light->TranslateLR(0.1f);
	//light->PositionAndOrient(light->C, tms[1].GetCenter(), V3(0.0f, 1.0f, 0.0f));
	//}
      {
        // Demo1: box moving away half buried parallel to ground plane
        //tms[1].SetCenter(V3(ppc->GetVD()[0]*10, ppc->GetVD()[1]*10, tms[0].GetCenter()[2]));
        light->C = V3(20.0f, 10.0f, 12.0f);
        light->PositionAndOrient(light->C, tms[1].GetCenter(), V3(0.0f, 1.0f, 0.0f));

        int fN = 150;
        for (int fi = 0; fi < fN; fi++) {
            fb->SetZB(0.0f);
			// set and view shadow map
			fb->SetSM(0.0f);
			tms[1].RenderShadowMap(light, fb);
			fbs->ViewShadowMap(fb->sm);
			fbs->redraw();

            fb->GetSpherical(ppc, fb3, tms[0].GetCenter(), light);
            // render virtual box
            tms[1].RenderFilled(ppc, fb);
            fb->redraw();
            Fl::check();
            V3 center = tms[1].GetCenter();
            //tms[1].SetCenter(center + V3(ppc->GetVD()[0]*0.5f, ppc->GetVD()[1]*0.5f, 0.0f));
            //tms[1].SetCenter(V3(ppc->GetVD()[0]*10, ppc->GetVD()[1]*10, tms[0].GetCenter()[2]+((float)2*fi/fN)));
            tms[1].SetCenter(V3(10.0f, 10.0f, tms[0].GetCenter()[2]+((float)3*fi/fN)));
            //tms[1].SetCenter(V3(10.0f, fi/10, tms[0].GetCenter()[2]));
        }
        Fl::check();
        return;
    }

    {
        // Demo 2: user moving forward, box stationary
        tms[1].SetCenter(V3(ppc->GetVD()[0]*10, ppc->GetVD()[1]*10, tms[0].GetCenter()[2]));
        light->C = V3(20.0f, 10.0f, 12.0f);
        light->PositionAndOrient(light->C, tms[1].GetCenter(), V3(0.0f, 1.0f, 0.0f));
        fb->SetSM(0.0f);
        tms[1].RenderShadowMap(light, fb);
        fbs->ViewShadowMap(fb->sm);
        fbs->redraw();

        int fN = 100;
        for (int fi = 0; fi < fN; fi++) {
            fb->SetZB(0.0f);
            fb->GetSpherical(ppc, fb3, tms[0].GetCenter(), light);
            // render virtual box
            tms[1].RenderFilled(ppc, fb);
            fb->redraw();
            Fl::check();
            //ppc->TranslateFB(-0.01f);
        }
        fb3->RenderPanoMap();
        fb3->redraw();
        Fl::check();
        return;
    }


}

// 3d geometry for panoramic images
void Scene::RenderJustPanorama() {

	fb3 = new FrameBuffer(u0, v0, 1400, 700);
	fb3->position(u0+fb->w+u0, v0);
	fb3->label("3rd person");
    fb3->show();
    ppc->Tilt(90.0f);
    ppc->Pan(-150.0f);

    RenderJustPanoramaLoop();
}

void Scene::RenderJustPanoramaLoop() {

	fb->SetBGR(0xFFFFFFFF);
	fb3->SetBGR(0xFFFFFFFF);
    fb3->SetTextureImage("img/pano.jpeg");

	fb3->ResetPanoMap();
    fb->GetSpherical(ppc, fb3);
    fb3->RenderPanoMap();
    fb->redraw();
    fb3->redraw();
    Fl::check();
}

// box + plane scene to show diff between izb and normal shadow mapping
void Scene::RenderShadowScene(int shadowType) {
    fb3 = new FrameBuffer(u0, v0, w, h);
    fb3->position(u0 + fb->w + u0, v0);
    fb3->label("3rd person");
    fb3->show();
    
    ppc3->C = V3(-7.0f, 8.0f, 5.0f);

    tms[0].SetPlane(V3(0.0f, -2.0f, -10.0f), 9.0f, 5.0f);
    tms[1].SetFullCube(V3(0.0f, 0.0f, -10.0f), 2.0f);

    light->C = V3(5.0f, 6.0f, -7.0f);
    light->PositionAndOrient(light->C, tms[1].GetCenter(), V3(0.0f, 1.0f, 0.0f));
    ppc3->PositionAndOrient(ppc3->C, tms[1].GetCenter(), V3(0.0f, 1.0f, 0.0f));

    if (shadowType == 0) {
        RenderSimpleLightScene();
    }
    else {
        RenderIZBScene();
    }
}

void Scene::RenderSimpleLightScene() {

    fb->SetBGR(0xFFFFFFFF);
    fb3->SetBGR(0xFFFFFFFF);
    fb->SetZB(0.00f);
    fb3->SetZB(0.00f);

    // set and view shadow map
    fb->SetSM(0.0f);
    fb3->SetSM(0.0f);
    tms[1].RenderShadowMap(light, fb);
    tms[1].RenderShadowMap(light, fb3);
    fbs->ViewShadowMap(fb->sm);
    fbs->redraw();

    V3 matColor(0.7f, 1.0f, 0.0f);
    float ka = 0.03f;
    float specularExp = 0.0f;

    fb3->Render3DSegment(ppc3, ppc->C, ppc->C + ppc->GetVD() * 100.0f,
        V3(1.0f, 0.0f, 0.0f), V3(0.0f, 0.0f, 0.0f));
    fb3->Render3DPoint(ppc3, ppc->C, V3(1.0f, 0.0f, 0.0f), 7.0f);
	fb3->Render3DSegment(ppc3, light->C, light->C + light->GetVD() * 100.0f, V3(1.0f, 1.0f, 0.0f),
		V3(1.0f, 1.0f, 0.0f));
	fb3->Render3DPoint(ppc3, light->C, V3(1.0f, 1.0f, 0.0f), 7.0f);

    for (int tmi = 0; tmi < tmsN; tmi++) {
        tms[tmi].RenderFillWithShadow(ppc, fb, matColor, light->GetVD(), ka, ambientFactor, specularExp, light);
        tms[tmi].RenderFillWithShadow(ppc3, fb3, matColor, light->GetVD(), ka, ambientFactor, specularExp, light);
    }


    fb->redraw();
    fb3->redraw();
    Fl::check();

}

void Scene::RenderIZBScene() {
    fb->SetBGR(0xFFFFFFFF);
    fb3->SetBGR(0xFFFFFFFF);
    fb->SetZB(0.00f);
    fb3->SetZB(0.00f);
    fb->ResetIZB();
    fb3->ResetIZB();

    // set and view shadow map
    fb->SetSM(0.0f);
    fb3->SetSM(0.0f);
    tms[1].RenderShadowMap(light, fb);
    tms[1].RenderShadowMap(light, fb3);
    fbs->ViewShadowMap(fb->sm);
    fbs->redraw();

    V3 matColor(0.7f, 1.0f, 0.0f);
    float ka = 0.03f;
    float specularExp = 0.0f;

    // irregular z buffering
    for (int tmi = 0; tmi < tmsN; tmi++) {
        tms[tmi].RenderUpdateIZB(ppc, fb, matColor, light->GetVD(), ka, ambientFactor, specularExp, light);
        tms[tmi].RenderUpdateIZB(ppc3, fb3, matColor, light->GetVD(), ka, ambientFactor, specularExp, light);
    }
    fb->UpdateIZB(ppc, light);
    fb3->UpdateIZB(ppc3, light);
    for (int tmi = 0; tmi < tmsN; tmi++) {
        tms[tmi].RenderIrregularShadows(light, fb);
        tms[tmi].RenderIrregularShadows(light, fb3);
    }

    fb3->Render3DSegment(ppc3, ppc->C, ppc->C + ppc->GetVD() * 100.0f,
        V3(1.0f, 0.0f, 0.0f), V3(0.0f, 0.0f, 0.0f));
    fb3->Render3DPoint(ppc3, ppc->C, V3(1.0f, 0.0f, 0.0f), 7.0f);
    fb3->Render3DSegment(ppc3, light->C, light->C + light->GetVD() * 100.0f, V3(1.0f, 1.0f, 0.0f),
        V3(1.0f, 1.0f, 0.0f));
    fb3->Render3DPoint(ppc3, light->C, V3(1.0f, 1.0f, 0.0f), 7.0f);

    fbizb = new FrameBuffer(u0, v0, 600, 600);
    fbizb->position(u0 + fb->w + u0, v0 + fb->h + v0);
    fbizb->label("Irregular z buffer");
    fbizb->ViewIZB(fb->izb);
    fbizb->show();
    fbizb->redraw();

    fb->redraw();
    fb3->redraw();
    Fl::check();

}

void Scene::DBG() {
    mode = 5;
    RenderReflectionDemoCube();
	return;
}

void Scene::ReposLight() {
	light->PositionAndOrient(light->C, tms[0].GetCenter(), V3(0.0f, 1.0f, 0.0f));
}

void Scene::Right() {
    ppc->TranslateLR(1.0f);
    Render();
}

void Scene::Left() {
    ppc->TranslateLR(-1.0f);
    Render();
}

void Scene::Up() {
    ppc->TranslateUD(1.0f);
    Render();
}

void Scene::Down() {
    ppc->TranslateUD(-1.0f);
    Render();
}

void Scene::Front() {
    ppc->TranslateFB(1.0f);
    Render();
}

void Scene::Back() {
    ppc->TranslateFB(-1.0f);
    Render();
}

void Scene::RotateX() {
        ppc->Rotate(V3(0.0f, 0.0f, 1.0f), 0.5f);
        Render();
}

void Scene::RotateXn() {
        ppc->Rotate(V3(0.0f, 0.0f, 1.0f), -0.5f);
        Render();
}

void Scene::Tilt() {
        ppc->Tilt(15.0f);
        Render();
}

void Scene::TiltN() {
        ppc->Tilt(-15.0f);
        Render();
}

void Scene::Pan() {
        ppc->Pan(-15.0f);
        Render();
}

void Scene::PanN() {
        ppc->Pan(15.0f);
        Render();
}

void Scene::ZoomIn() {
	ppc->Zoom(1.1f);
	Render();
}

void Scene::ZoomOut() {
	ppc->Zoom(0.9f);
	Render();
}

void Scene::LightR() {
    light->TranslateLR(1.0f);
    light->Pan(0.2f);
    ReposLight();
    RenderLSM(fb);
    Render();
}

void Scene::LightL() {
    light->TranslateLR(-1.0f);
    light->Pan(-0.2f);
    ReposLight();
    RenderLSM(fb);
    Render();
}

void Scene::LightU() {
    light->TranslateUD(1.0f);
    ReposLight();
    RenderLSM(fb);
    Render();
}

void Scene::LightD() {
    light->TranslateUD(-1.0f);
    ReposLight();
    RenderLSM(fb);
    Render();
}

void Scene::NewButton() {
	cerr << "INFO: pressed New button on GUI" << endl;
}

void Scene::SetMode(int newMode) {
    mode = newMode;
}
