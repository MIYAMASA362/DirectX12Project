#ifndef __CORE_INPUT_H__
#define __CORE_INPUT_H__

#define DIRECTINPUT_VERSION 0x0800

// Direct Input
#include <dinput.h>
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

// XInput
//https://docs.microsoft.com/en-us/windows/win32/xinput/programming-guide
#include <Xinput.h>
#pragma comment(lib,"Xinput.lib")

#include "Singleton.h"

struct Float3
{
	union
	{
		float f[3];
		struct 
		{
			float x;
			float y;
			float z;
		};
	};
	Float3(float x, float y, float z)
		: x(x)
		, y(y)
		, z(z)
	{

	}
};

class Input final : public Common::Singleton<Input>
{
public:
	Input();
	virtual ~Input();

	void onInit();
	void onUpdate();
	void onDestory();

	bool getKey(BYTE key);
	bool getKeyDown(BYTE key);
	bool getKeyUp(BYTE key);

	void setCursorLoop(bool isLoop) { mCursorLoop = isLoop; }

	// Left:0 Right:1 Center:2 
	bool getMouseButton(BYTE btn);
	bool getMouseButtonDown(BYTE btn);
	bool getMouseButtonUp(BYTE btn);
	POINT getMousePos() const { return mMousePos; }
	Float3 getAcceleration() { return Float3((float)mMouse.lX, (float)mMouse.lY, (float)mMouse.lZ); }

private:
	LPDIRECTINPUT8 mlpInput;

	BYTE mOldKeyState[256];
	BYTE mKeyState[256];

	LPDIRECTINPUTDEVICE8 mlpMouse;
	DIMOUSESTATE2 mMouse;
	POINT mMousePos;
	BYTE mOldMouseButton[8];
	bool mCursorLoop;
};
#endif