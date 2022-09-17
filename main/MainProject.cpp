#include "stdafx.h"
#include "MainProject.h"
#include "Renderer.h"
#include "Camera.h"

#include "Input.h"

MainProject::MainProject(UINT width, UINT height, std::wstring title)
	: AppProject(width, height, title)
	, mpCamera(nullptr)
	, mpPlane(nullptr)
	, mpRenderer(nullptr)
{
	Input::createInstance();
}

MainProject::~MainProject()
{
	Input::destoryInstance();
}

void MainProject::onInit()
{
	Input::getInstance()->onInit();

	mpCamera = new Camera();
	mpCamera->getTransform()->setLocalPosition({ 0, 0, -10 });

	mpPlane = new Plane();

	mpRenderer = new Renderer();

	mpRenderer->onInit();

	mpCamera->setup();
	mpPlane->onSetup();
}

void MainProject::onUpdate()
{
	Input::getInstance()->onUpdate();

	mpCamera->update();
	mpPlane->onUpdate();
}

void MainProject::onDraw()
{
	mpCamera->onRender();
	mpPlane->onRender();

	mpRenderer->onRender(mpCamera);
}

void MainProject::onDestroy()
{
	if (mpRenderer != nullptr) {
		mpRenderer->onDestroy();
		delete mpRenderer;
	}

	Input::getInstance()->onDestory();
}