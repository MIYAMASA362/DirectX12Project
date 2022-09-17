#include "stdafx.h"
#include "AppProject.h"

AppProject::AppProject(UINT width, UINT height, std::wstring title) 
	: mWidth(width)
	, mHeight(height)
	, mIsExit(false)
	, mTitle(title)
{

}

AppProject::~AppProject()
{
}

void AppProject::setCustomWindowText(LPCWSTR text)
{
	std::wstring output = mTitle + text;
	SetWindowText(Application::getHwnd(), output.c_str());
}
