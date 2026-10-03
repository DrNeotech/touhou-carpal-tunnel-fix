#include "dinputproxy.h"

HMODULE hLThis = NULL;
HMODULE hL = NULL;
FARPROC p[5] = {0};

BOOL WINAPI DllMain(HINSTANCE hInst, DWORD reason, LPVOID reserved)
{
	(void)reserved;

	if (reason == DLL_PROCESS_ATTACH)
	{
		char szPath[MAX_PATH];
		UINT len;

		DisableThreadLibraryCalls(hInst);

		len = GetSystemDirectoryA(szPath, sizeof(szPath));
		if (!len || len + sizeof("\\dinput8.dll") > sizeof(szPath))
			return FALSE;

		lstrcatA(szPath, "\\dinput8.dll");

		hLThis = hInst;
		hL = LoadLibraryA(szPath);
		if (!hL)
			return FALSE;

		p[0] = GetProcAddress(hL, "DirectInput8Create");
		p[1] = GetProcAddress(hL, "DllCanUnloadNow");
		p[2] = GetProcAddress(hL, "DllGetClassObject");
		p[3] = GetProcAddress(hL, "DllRegisterServer");
		p[4] = GetProcAddress(hL, "DllUnregisterServer");
	}
	else if (reason == DLL_PROCESS_DETACH)
	{
		if (hL)
		{
			FreeLibrary(hL);
			hL = NULL;
		}
	}

	return TRUE;
}


HRESULT WINAPI __E__0__(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID *ppvOut, LPUNKNOWN punkOuter)
{
	DirectInput8Create_t create = (DirectInput8Create_t)p[0];
	LPVOID obj = NULL;
	HRESULT ret;

	if (!create || !ppvOut)
		return DIERR_INVALIDPARAM;

	ret = create(hinst, dwVersion, riidltf, &obj, punkOuter);
	if (FAILED(ret))
		return ret;

	if (IsEqualIID(riidltf, &IID_IDirectInput8A))
	{
		proxy_IDirectInput *proxyDI;
		HRESULT hr = proxy_IDirectInput_Create((IDirectInput8A *)obj, &proxyDI);
		if (FAILED(hr))
		{
			((IUnknown *)obj)->lpVtbl->Release((IUnknown *)obj);
			return hr;
		}
		obj = proxyDI;
	}

	*ppvOut = obj;
	return ret;
}

HRESULT WINAPI __E__1__(void)
{
	HRESULT (WINAPI *fn)(void) = (HRESULT (WINAPI *)(void))p[1];
	return fn ? fn() : S_FALSE;
}

HRESULT WINAPI __E__2__(REFCLSID rclsid, REFIID riid, LPVOID *ppv)
{
	HRESULT (WINAPI *fn)(REFCLSID, REFIID, LPVOID *) = (HRESULT (WINAPI *)(REFCLSID, REFIID, LPVOID *))p[2];
	return fn ? fn(rclsid, riid, ppv) : CLASS_E_CLASSNOTAVAILABLE;
}

HRESULT WINAPI __E__3__(void)
{
	HRESULT (WINAPI *fn)(void) = (HRESULT (WINAPI *)(void))p[3];
	return fn ? fn() : E_FAIL;
}

HRESULT WINAPI __E__4__(void)
{
	HRESULT (WINAPI *fn)(void) = (HRESULT (WINAPI *)(void))p[4];
	return fn ? fn() : E_FAIL;
}