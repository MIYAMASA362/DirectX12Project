#ifndef __CORE_APPLICATION_H__
#define __CORE_APPLICATION_H__
#include "AppProject.h"

class AppRroject;

class Application
{
public:
	static int run(class AppProject* pProject, HINSTANCE hInstance, int nComdShow);

	static HWND getHwnd() { return mhWnd; }
	static std::wstring getAssetFullPath(LPCWSTR assetName);

protected:
	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	static HWND mhWnd;

};
#endif