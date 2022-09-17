#include "stdafx.h"

#include "Application.h"
#include "MainProject.h"

#include "Math.h"

//=============================================================================
// WinMain
//=============================================================================
int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	MainProject project(1280, 720, L"DirectX12");
	return Application::run(&project, hInstance, nCmdShow);
}