#ifndef __MAINPROJECT_H__
#define __MAINPROJECT_H__
#include "AppProject.h"

using namespace DirectX;

class MainProject final : public AppProject
{
public:
	MainProject(UINT width, UINT height, std::wstring title);
	~MainProject();

	void onInit() override;
	void onUpdate() override;
	void onDraw() override;
	void onDestroy() override;

private:
	class Camera* mpCamera;
	class Plane* mpPlane;
	class Renderer* mpRenderer;
};
#endif /* __MAINPROJECT_H__ */