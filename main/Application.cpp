#include "stdafx.h"
#include "DirectXHelper.h"

#include "Application.h"
#include "AppProject.h"

HWND Application::mhWnd = nullptr;

DWORD WINAPI GameThread(LPVOID lpParam)
{
	DWORD mainThreadId = GetWindowThreadProcessId(Application::getHwnd(), NULL);
	DWORD selfThreadId = GetCurrentThreadId();
	AttachThreadInput(mainThreadId, selfThreadId, TRUE);

	AppProject* pProject = (AppProject*)lpParam;
	while (!pProject->getExit())
	{
		pProject->onUpdate();

		pProject->onDraw();
	}

	AttachThreadInput(mainThreadId, selfThreadId, FALSE);

	ExitThread(TRUE);
}

int Application::run(AppProject* pProject, HINSTANCE hInstance, int nComdShow)
{
	int argc;
	LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);

	LocalFree(argv);

	// ウィンドウ生成
	WNDCLASSEX wnd = {};
	wnd.cbSize = sizeof(WNDCLASSEX);
	wnd.style = CS_CLASSDC;
	wnd.lpfnWndProc = WindowProc;
	wnd.hInstance = hInstance;
	wnd.hCursor = LoadCursor(NULL, IDC_ARROW);
	wnd.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wnd.lpszClassName = L"MainWindow";

	// ウィンドウの登録
	if (!RegisterClassEx(&wnd)) {
		return 0;
	}

	RECT rect = {
		0,
		0,
		static_cast<LONG>(pProject->getWidth()),
		static_cast<LONG>(pProject->getHeight())
	};
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

	// ウィンドウ生成
	mhWnd = CreateWindow(
		wnd.lpszClassName,
		pProject->getTitle(),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		rect.right - rect.left,
		rect.bottom - rect.top,
		nullptr,
		nullptr,
		hInstance,
		pProject
	);

	// AppProject::onInit()
	pProject->onInit();

	DWORD gameThreadID;
	HANDLE hGameThread = CreateThread(
		NULL,
		0,
		GameThread,
		pProject,
		0,
		&gameThreadID
	);

	if (hGameThread == NULL) {
		return 0;
	}
	SetThreadDescription(hGameThread, L"GameThread");

	BYTE key[256];
	if (!GetKeyboardState(key)) {
		OutputDebugStringA("Error");
	}

	ShowWindow(mhWnd, nComdShow);

	MSG msg = {};
	while (true)
	{
		BOOL message = GetMessage(&msg, NULL, 0, 0);
		BOOL IsQuit = (message == 0);
		BOOL IsError = (message == -1);

		if (msg.message == WM_QUIT) {
			break;
		}

		if (IsQuit || IsError)
			break;

		DispatchMessage(&msg);
		TranslateMessage(&msg);
	}

	if (hGameThread != NULL) {
		DWORD result;
		pProject->setExit(true);
		while (true)
		{
			GetExitCodeThread(hGameThread, &result);
			if (STILL_ACTIVE != result) {
				CloseHandle(hGameThread);
				break;
			}
		}
	}

	// AppProject::onDestroy()
	pProject->onDestroy();

	return (int)msg.wParam;
}

std::wstring Application::getAssetFullPath(LPCWSTR assetName)
{
	WCHAR assetsPath[512];
	getAssetsPath(assetsPath, _countof(assetsPath));
	std::wstring mAssetsPath = assetsPath;
	return mAssetsPath + assetName;
}

LRESULT Application::WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	AppProject* pProject = reinterpret_cast<AppProject*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

	switch (message) {
	case WM_CREATE:
	{
		LPCREATESTRUCT pCreateStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
		SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pCreateStruct->lpCreateParams));
	}
	return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}
