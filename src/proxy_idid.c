#include "dinputproxy.h"
#include <stdbool.h>
#include <stdio.h>
#include <windows.h>

#define PDID(self) ((proxy_IDirectInputDevice *)(self))
#define REAL(self) (PDID(self)->did)

typedef struct {
    int heldframes;
    bool shooting;
    bool charging;
} State;

State gamestate = { 0, false, false };

HRESULT STDMETHODCALLTYPE proxy_IDirectInputDevice_GetDeviceData(IDirectInputDevice8A *self, DWORD cbObjectData, LPDIDEVICEOBJECTDATA rgdod, LPDWORD pdwInOut, DWORD dwFlags)
{
	proxy_IDirectInputDevice *proxydid = PDID(self);
	DWORD capacity = pdwInOut ? *pdwInOut : 0;
	HRESULT ret;

	ret = proxydid->did->lpVtbl->GetDeviceData(proxydid->did, cbObjectData, rgdod, pdwInOut, dwFlags);

	if (proxydid->bKeyboard && SUCCEEDED(ret) && rgdod && pdwInOut &&
	    !(dwFlags & DIGDD_PEEK) && cbObjectData == sizeof(DIDEVICEOBJECTDATA))
	{
		KeyboardHandler(proxydid, rgdod, pdwInOut, capacity);
	}

	return ret;
}

HRESULT STDMETHODCALLTYPE proxy_IDirectInputDevice_QueryInterface(IDirectInputDevice8A *self, REFIID riid, LPVOID *ppvObj)
{
	if (!ppvObj)
		return E_POINTER;

	if (IsEqualIID(riid, &IID_IUnknown) || IsEqualIID(riid, &IID_IDirectInputDevice8A))
	{
		*ppvObj = self;
		proxy_IDirectInputDevice_AddRef(self);
		return S_OK;
	}

	return REAL(self)->lpVtbl->QueryInterface(REAL(self), riid, ppvObj);
}

ULONG STDMETHODCALLTYPE proxy_IDirectInputDevice_AddRef(IDirectInputDevice8A *self)
{
	return (ULONG)InterlockedIncrement(&PDID(self)->refcount);
}

ULONG STDMETHODCALLTYPE proxy_IDirectInputDevice_Release(IDirectInputDevice8A *self)
{
	proxy_IDirectInputDevice *proxydid = PDID(self);
	LONG ref = InterlockedDecrement(&proxydid->refcount);

	if (ref == 0)
	{
		proxydid->did->lpVtbl->Release(proxydid->did);
		free(proxydid);
	}

	return (ULONG)ref;
}

HRESULT STDMETHODCALLTYPE proxy_IDirectInputDevice_GetCapabilities(IDirectInputDevice8A *self, LPDIDEVCAPS lpDIDevCaps)
{
	return REAL(self)->lpVtbl->GetCapabilities(REAL(self), lpDIDevCaps);
}

HRESULT STDMETHODCALLTYPE proxy_IDirectInputDevice_EnumObjects(IDirectInputDevice8A *self, LPDIENUMDEVICEOBJECTSCALLBACKA lpCallback, LPVOID pvRef, DWORD dwFlags)
{
	return REAL(self)->lpVtbl->EnumObjects(REAL(self), lpCallback, pvRef, dwFlags);
}

HRESULT STDMETHODCALLTYPE proxy_IDirectInputDevice_GetProperty(IDirectInputDevice8A *self, REFGUID rguidProp, LPDIPROPHEADER pdiph)
{
	return REAL(self)->lpVtbl->GetProperty(REAL(self), rguidProp, pdiph);
}

HRESULT STDMETHODCALLTYPE proxy_IDirectInputDevice_SetProperty(IDirectInputDevice8A *self, REFGUID rguidProp, LPCDIPROPHEADER pdiph)
{
	return REAL(self)->lpVtbl->SetProperty(REAL(self), rguidProp, pdiph);
}

HRESULT STDMETHODCALLTYPE proxy_IDirectInputDevice_Acquire(IDirectInputDevice8A *self)
{
	return REAL(self)->lpVtbl->Acquire(REAL(self));
}

HRESULT STDMETHODCALLTYPE proxy_IDirectInputDevice_Unacquire(IDirectInputDevice8A *self)
{
	return REAL(self)->lpVtbl->Unacquire(REAL(self));
}

HRESULT STDMETHODCALLTYPE proxy_IDirectInputDevice_GetDeviceState(IDirectInputDevice8A *self, DWORD cbData, LPVOID lpvData)
{
	HRESULT hr = REAL(self)->lpVtbl->GetDeviceState(REAL(self), cbData, lpvData);

	if (SUCCEEDED(hr) && cbData == 256 && lpvData != NULL) {
		BYTE* keys = (BYTE*)lpvData;

		bool zDown = (keys[DIK_Z] & 0x80) != 0;
		bool xDown = (keys[DIK_X] & 0x80) != 0;
		bool cDown = (keys[DIK_C] & 0x80) != 0;

		gamestate.charging = xDown;
		gamestate.shooting = zDown && !xDown;

		gamestate.heldframes = (gamestate.shooting) ? ++gamestate.heldframes : 0;

		if ((gamestate.heldframes % 2) != 0) {
			keys[DIK_Z] = 0x00;
		}

		keys[DIK_X] = 0x00; // adding this makes it work flawlessly for touhou 9. happy little accident.

		if (cDown) {
			keys[DIK_X] = 0x80;
		}
	}
	return hr;
}

HRESULT STDMETHODCALLTYPE proxy_IDirectInputDevice_SetDataFormat(IDirectInputDevice8A *self, LPCDIDATAFORMAT lpdf)
{
	return REAL(self)->lpVtbl->SetDataFormat(REAL(self), lpdf);
}

HRESULT STDMETHODCALLTYPE proxy_IDirectInputDevice_SetEventNotification(IDirectInputDevice8A *self, HANDLE hEvent)
{
	return REAL(self)->lpVtbl->SetEventNotification(REAL(self), hEvent);
}

HRESULT STDMETHODCALLTYPE proxy_IDirectInputDevice_SetCooperativeLevel(IDirectInputDevice8A *self, HWND hwnd, DWORD dwFlags)
{
	return REAL(self)->lpVtbl->SetCooperativeLevel(REAL(self), hwnd, dwFlags);
}

HRESULT STDMETHODCALLTYPE proxy_IDirectInputDevice_GetObjectInfo(IDirectInputDevice8A *self, LPDIDEVICEOBJECTINSTANCEA pdidoi, DWORD dwObj, DWORD dwHow)
{
	return REAL(self)->lpVtbl->GetObjectInfo(REAL(self), pdidoi, dwObj, dwHow);
}

HRESULT STDMETHODCALLTYPE proxy_IDirectInputDevice_GetDeviceInfo(IDirectInputDevice8A *self, LPDIDEVICEINSTANCEA pdidi)
{
	return REAL(self)->lpVtbl->GetDeviceInfo(REAL(self), pdidi);
}

HRESULT STDMETHODCALLTYPE proxy_IDirectInputDevice_RunControlPanel(IDirectInputDevice8A *self, HWND hwndOwner, DWORD dwFlags)
{
	return REAL(self)->lpVtbl->RunControlPanel(REAL(self), hwndOwner, dwFlags);
}

HRESULT STDMETHODCALLTYPE proxy_IDirectInputDevice_Initialize(IDirectInputDevice8A *self, HINSTANCE hinst, DWORD dwVersion, REFGUID rguid)
{
	return REAL(self)->lpVtbl->Initialize(REAL(self), hinst, dwVersion, rguid);
}

HRESULT STDMETHODCALLTYPE proxy_IDirectInputDevice_CreateEffect(IDirectInputDevice8A *self, REFGUID rguid, LPCDIEFFECT lpeff, LPDIRECTINPUTEFFECT *ppdeff, LPUNKNOWN punkOuter)
{
	return REAL(self)->lpVtbl->CreateEffect(REAL(self), rguid, lpeff, ppdeff, punkOuter);
}

HRESULT STDMETHODCALLTYPE proxy_IDirectInputDevice_EnumEffects(IDirectInputDevice8A *self, LPDIENUMEFFECTSCALLBACKA lpCallback, LPVOID pvRef, DWORD dwEffType)
{
	return REAL(self)->lpVtbl->EnumEffects(REAL(self), lpCallback, pvRef, dwEffType);
}

HRESULT STDMETHODCALLTYPE proxy_IDirectInputDevice_GetEffectInfo(IDirectInputDevice8A *self, LPDIEFFECTINFOA pdei, REFGUID rguid)
{
	return REAL(self)->lpVtbl->GetEffectInfo(REAL(self), pdei, rguid);
}

HRESULT STDMETHODCALLTYPE proxy_IDirectInputDevice_GetForceFeedbackState(IDirectInputDevice8A *self, LPDWORD pdwOut)
{
	return REAL(self)->lpVtbl->GetForceFeedbackState(REAL(self), pdwOut);
}

HRESULT STDMETHODCALLTYPE proxy_IDirectInputDevice_SendForceFeedbackCommand(IDirectInputDevice8A *self, DWORD dwFlags)
{
	return REAL(self)->lpVtbl->SendForceFeedbackCommand(REAL(self), dwFlags);
}

HRESULT STDMETHODCALLTYPE proxy_IDirectInputDevice_EnumCreatedEffectObjects(IDirectInputDevice8A *self, LPDIENUMCREATEDEFFECTOBJECTSCALLBACK lpCallback, LPVOID pvRef, DWORD fl)
{
	return REAL(self)->lpVtbl->EnumCreatedEffectObjects(REAL(self), lpCallback, pvRef, fl);
}

HRESULT STDMETHODCALLTYPE proxy_IDirectInputDevice_Escape(IDirectInputDevice8A *self, LPDIEFFESCAPE pesc)
{
	return REAL(self)->lpVtbl->Escape(REAL(self), pesc);
}

HRESULT STDMETHODCALLTYPE proxy_IDirectInputDevice_Poll(IDirectInputDevice8A *self)
{
	return REAL(self)->lpVtbl->Poll(REAL(self));
}

HRESULT STDMETHODCALLTYPE proxy_IDirectInputDevice_SendDeviceData(IDirectInputDevice8A *self, DWORD cbObjectData, LPCDIDEVICEOBJECTDATA rgdod, LPDWORD pdwInOut, DWORD fl)
{
	return REAL(self)->lpVtbl->SendDeviceData(REAL(self), cbObjectData, rgdod, pdwInOut, fl);
}

HRESULT STDMETHODCALLTYPE proxy_IDirectInputDevice_EnumEffectsInFile(IDirectInputDevice8A *self, LPCSTR lpszFileName, LPDIENUMEFFECTSINFILECALLBACK pec, LPVOID pvRef, DWORD dwFlags)
{
	return REAL(self)->lpVtbl->EnumEffectsInFile(REAL(self), lpszFileName, pec, pvRef, dwFlags);
}

HRESULT STDMETHODCALLTYPE proxy_IDirectInputDevice_WriteEffectToFile(IDirectInputDevice8A *self, LPCSTR lpszFileName, DWORD dwEntries, LPDIFILEEFFECT rgDiFileEft, DWORD dwFlags)
{
	return REAL(self)->lpVtbl->WriteEffectToFile(REAL(self), lpszFileName, dwEntries, rgDiFileEft, dwFlags);
}

HRESULT STDMETHODCALLTYPE proxy_IDirectInputDevice_BuildActionMap(IDirectInputDevice8A *self, LPDIACTIONFORMATA lpdiaf, LPCSTR lpszUserName, DWORD dwFlags)
{
	return REAL(self)->lpVtbl->BuildActionMap(REAL(self), lpdiaf, lpszUserName, dwFlags);
}

HRESULT STDMETHODCALLTYPE proxy_IDirectInputDevice_SetActionMap(IDirectInputDevice8A *self, LPDIACTIONFORMATA lpdiaf, LPCSTR lpszUserName, DWORD dwFlags)
{
	return REAL(self)->lpVtbl->SetActionMap(REAL(self), lpdiaf, lpszUserName, dwFlags);
}

HRESULT STDMETHODCALLTYPE proxy_IDirectInputDevice_GetImageInfo(IDirectInputDevice8A *self, LPDIDEVICEIMAGEINFOHEADERA lpdiDevImageInfoHeader)
{
	return REAL(self)->lpVtbl->GetImageInfo(REAL(self), lpdiDevImageInfoHeader);
}