#ifndef __CORE_APPROJECT_H__
#define __CORE_APPROJECT_H__
#include "Application.h"

class AppProject
{
public:
	AppProject(UINT width, UINT height, std::wstring title);
	virtual ~AppProject();

	virtual void onInit() =0;
	virtual void onUpdate() =0;
	virtual void onDraw() =0;
	virtual void onDestroy() =0;

	// Accessors
	UINT getWidth()			const { return mWidth; }
	UINT getHeight()		const { return mHeight; }

	const WCHAR* getTitle() const { return mTitle.c_str(); }

	bool getExit() { return mIsExit; }
	void setExit(bool exit) { mIsExit = exit; }

protected:
	void setCustomWindowText(LPCWSTR text);

	UINT mWidth;
	UINT mHeight;

	bool mIsExit;

private:
	// Window title.
	std::wstring mTitle;
};
#endif