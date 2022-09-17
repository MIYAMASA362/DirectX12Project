#include "stdafx.h"
#include "Application.h"
#include "Input.h"

DEFINE_SIGLETON(Input);

Input::Input()
	: mlpInput(nullptr)
	, mOldKeyState()
	, mKeyState()
	, mCursorLoop(false)
{

}

Input::~Input()
{

}

void Input::onInit()
{
	HRESULT hr;
	HINSTANCE hInstance = (HINSTANCE)GetWindowLongPtr(Application::getHwnd(), GWLP_HINSTANCE);

	// Direct Input
	{
		hr = DirectInput8Create(
			hInstance,
			DIRECTINPUT_VERSION,
			IID_IDirectInput8,
			(void**)&mlpInput,
			NULL
		);

		if (FAILED(hr)) {
			mlpInput->Release();
			MessageBox(NULL, L"Inputデバイスの生成に失敗しました。", L"エラー", MB_OK);
			return;
		}

		hr = mlpInput->CreateDevice(
			GUID_SysMouse,
			&mlpMouse,
			NULL
		);

		if (FAILED(hr)) {
			mlpMouse->Release();
			MessageBox(NULL, L"Mouseデバイスの生成に失敗しました。", L"エラー", MB_OK);
			return;
		}

		hr = mlpMouse->SetDataFormat(&c_dfDIMouse2);
		if (FAILED(hr)) {
			mlpMouse->Release();
			MessageBox(NULL, L"Mouseデバイスの生成に失敗しました。", L"エラー", MB_OK);
			return;
		}

		hr = mlpMouse->SetCooperativeLevel(Application::getHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
		if (FAILED(hr)) {
			mlpMouse->Release();
			MessageBox(NULL, L"Mouseデバイスの生成に失敗しました。", L"エラー", MB_OK);
			return;
		}
		mlpMouse->Acquire();
	}

	// XInput
	{
		DWORD result = ERROR;
		for (DWORD i = 0; i < XUSER_MAX_COUNT; ++i) {

			// GamePad
			XINPUT_STATE state;
			result = XInputGetState(i, &state);
			if (result == ERROR_SUCCESS)
			{
				// Controller is connected.
				XINPUT_GAMEPAD& gamePad = state.Gamepad;
			}
			else
			{
				// Controller is not connected.
			}

			// Stroke
			XINPUT_KEYSTROKE stroke;
			result = XInputGetKeystroke(i,0,&stroke);
			if (result == ERROR_SUCCESS) 
			{

			}
			else
			{

			}

			//
		}
	}

	memset(mOldKeyState,0,256);
	memset(mKeyState,0,256);

	memset(mOldMouseButton, 0, 8);

	return;
}

void Input::onUpdate()
{
	// キーボード
	{
		memcpy_s(mOldKeyState, 256, mKeyState, 256);
		if (!GetKeyboardState(mKeyState)) {
			OutputDebugStringA("Input: Could not getting the Virtual Keyboard State\n");
		}
	}

	// マウス
	{
		memcpy_s(mOldMouseButton, 8, mMouse.rgbButtons, 8);

		GetCursorPos(&mMousePos);
		if (mCursorLoop) {
			WINDOWINFO info;
			GetWindowInfo(Application::getHwnd(), &info);
			//--- 左右 -------------------------------------------------------------------
			if ((UINT)mMousePos.x <= info.rcWindow.left + info.cxWindowBorders)
				SetCursorPos(info.rcWindow.right - info.cxWindowBorders, mMousePos.y);
			else if ((UINT)mMousePos.x >= info.rcWindow.right - info.cxWindowBorders)
				SetCursorPos(info.rcClient.left + info.cxWindowBorders, mMousePos.y);
			//--- 上下 -------------------------------------------------------------------
			if ((UINT)mMousePos.y <= info.rcWindow.top + info.cyWindowBorders)
				SetCursorPos(mMousePos.x, info.rcWindow.bottom - info.cyWindowBorders);
			else if ((UINT)mMousePos.y >= info.rcWindow.bottom - info.cyWindowBorders)
				SetCursorPos(mMousePos.x, info.rcWindow.top + info.cyWindowBorders);
		}
		ScreenToClient(Application::getHwnd(), &mMousePos);

		if (FAILED(mlpMouse->GetDeviceState(sizeof(DIMOUSESTATE2), &mMouse))) {
			mlpMouse->Acquire();
		}
	}
}

void Input::onDestory()
{
	if (mlpInput != nullptr) {
		mlpInput->Release();
		mlpInput = nullptr;
	}
}

// 0x80は先頭ビットのマスク
bool Input::getKey(BYTE key)
{
	return (mKeyState[key] & 0x80) != 0 ? true : false;
}

bool Input::getKeyDown(BYTE key)
{
	return (mKeyState[key] & 0x80) && !(mOldKeyState[key] & 0x80);
}

bool Input::getKeyUp(BYTE key)
{
	return !(mKeyState[key] & 0x80) && (mOldKeyState[key] & 0x80);
}

bool Input::getMouseButton(BYTE btn)
{
	if (btn > 2) return false;
	return (mMouse.rgbButtons[btn] & 0x80);
}

bool Input::getMouseButtonDown(BYTE btn)
{
	if (btn > 2) return false;
	return (mMouse.rgbButtons[btn] & 0x80) && !(mOldMouseButton[btn] & 0x80);
}

bool Input::getMouseButtonUp(BYTE btn)
{
	if (btn > 2) return false;
	return !(mMouse.rgbButtons[btn] & 0x80) && (mOldMouseButton[btn] & 0x80);
}
