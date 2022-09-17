#pragma once

#include <stdexcept>

using Microsoft::WRL::ComPtr;

inline std::string HrToString(HRESULT hr)
{
	char str[64] = {};
	sprintf_s(str, "HRESULT of 0x%08X", static_cast<UINT>(hr));
	return std::string(str);
}

class HrException : public std::runtime_error 
{
public:
	HrException(HRESULT hr) : std::runtime_error(HrToString(hr)), mHr(hr) {}
	HRESULT Error() const { return mHr; }
private:
	const HRESULT mHr;
};

#define SAFE_RELEASE(p) if(p) (p)->Release()

inline void ThrowIfFailed(HRESULT hr) 
{
	if (FAILED(hr)) throw std::exception();
}

inline void getAssetsPath(_Out_writes_(pathSize) WCHAR* path, UINT pathSize)
{
	if (path == nullptr)
	{
		throw std::exception();
	}

	// Gets the path of the module used to create the calling process
	// 呼び出し側プロセスを作成するために使われたモジュールのパスを取得します
	DWORD size = GetModuleFileName(nullptr, path, pathSize);
	if (size == 0 || size == pathSize)
	{
		// Method failed or path was truncated.
		throw std::exception();
	}

	WCHAR* lastSlash = wcsrchr(path, L'\\');
	if (lastSlash)
	{
		*(lastSlash + 1) = L'\0';
	}
}