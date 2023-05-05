#pragma once

#include "gui.h"
#include "framebuffer.h"
#include "ppc.h"
#include "tm.h"
#include "light.h"

class Scene {
public:

	GUI *gui;
	FrameBuffer *fb, *fb3, *fbs, *fbizb;
	PPC *ppc, *ppc3;
	TM *tms;
	Light* light;
	int tmsN;
    int mode;
	float ambientFactor;
	Scene();
    void RenderLSM(FrameBuffer* rfb);
	void Render(PPC* rppc, FrameBuffer* rfb);
	void Render();
	void DBG();
	void NewButton();
    void Right();
    void Left();
    void Up();
    void Down();
    void Front();
    void Back();
    void RotateX();
    void RotateXn();
    void Tilt();
    void TiltN();
    void Pan();
    void PanN();
    void ZoomIn();
    void ZoomOut();
    void LightR();
    void LightL();
    void LightU();
    void LightD();
    void ReposLight();
    void RenderWaterReflectionDemo();
    void RenderJustPanorama();
    void RenderJustPanoramaLoop();
	void RenderSimpleLightScene();
	void RenderIZBScene();
    void RenderShadowScene(int shadowType);
    void RenderGroundPlaneShadows();
	void RenderReflectionDemo();
    void ReflectionDemoLoop();
    void RenderReflectionDemoCube();
    void SetMode(int newMode);
};

extern Scene *scene;