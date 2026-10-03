#ifndef DINPUTPROXY_H
#define DINPUTPROXY_H

#define WIN32_LEAN_AND_MEAN
#ifndef DIRECTINPUT_VERSION
#define DIRECTINPUT_VERSION 0x0800
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <windows.h>
#include <dinput.h>

typedef unsigned long uint32;

typedef struct
{
	DWORD offset;
	DWORD data;
} keyqueueitem_t;

typedef struct
{
	IDirectInput8AVtbl *lpVtbl;
	IDirectInput8AVtbl vtbl;
	IDirectInput8A *di;
	LONG refcount;
} proxy_IDirectInput;

typedef struct
{
	IDirectInputDevice8AVtbl *lpVtbl;
	IDirectInputDevice8AVtbl vtbl;
	IDirectInputDevice8A *did;
	LONG refcount;
	BOOL bKeyboard;
	BOOL bMouse;
	BOOL bControlDown;
	uint32 sequence;
} proxy_IDirectInputDevice;

typedef HRESULT (WINAPI *DirectInput8Create_t)(HINSTANCE, DWORD, REFIID, LPVOID *, LPUNKNOWN);

extern HMODULE hLThis;
extern HMODULE hL;
extern FARPROC p[5];

HRESULT KeyboardHandler(proxy_IDirectInputDevice *proxydid, LPDIDEVICEOBJECTDATA rgdod, LPDWORD pdwInOut, DWORD dwCapacity);
void AddKeyToQueue(DWORD offset, DWORD data);
BOOL GetKeyFromQueue(keyqueueitem_t *key);
void PressKeyQueue(DWORD key);
void PressKeysString(const char *str);
void SendBufferedKeys(proxy_IDirectInputDevice *proxydid, LPDIDEVICEOBJECTDATA rgdod, LPDWORD pdwInOut, DWORD dwCapacity);

HRESULT proxy_IDirectInput_Create(IDirectInput8A *real, proxy_IDirectInput **out);

HRESULT STDMETHODCALLTYPE proxy_IDirectInput_QueryInterface(IDirectInput8A *self, REFIID riid, LPVOID *ppvObj);
ULONG   STDMETHODCALLTYPE proxy_IDirectInput_AddRef(IDirectInput8A *self);
ULONG   STDMETHODCALLTYPE proxy_IDirectInput_Release(IDirectInput8A *self);
HRESULT STDMETHODCALLTYPE proxy_IDirectInput_CreateDevice(IDirectInput8A *self, REFGUID rguid, LPDIRECTINPUTDEVICE8A *lplpDevice, LPUNKNOWN pUnkOuter);
HRESULT STDMETHODCALLTYPE proxy_IDirectInput_EnumDevices(IDirectInput8A *self, DWORD dwDevType, LPDIENUMDEVICESCALLBACKA lpCallback, LPVOID pvRef, DWORD dwFlags);
HRESULT STDMETHODCALLTYPE proxy_IDirectInput_GetDeviceStatus(IDirectInput8A *self, REFGUID rguidInstance);
HRESULT STDMETHODCALLTYPE proxy_IDirectInput_RunControlPanel(IDirectInput8A *self, HWND hwndOwner, DWORD dwFlags);
HRESULT STDMETHODCALLTYPE proxy_IDirectInput_Initialize(IDirectInput8A *self, HINSTANCE hinst, DWORD dwVersion);
HRESULT STDMETHODCALLTYPE proxy_IDirectInput_FindDevice(IDirectInput8A *self, REFGUID rguid, LPCSTR pszName, LPGUID pguidInstance);
HRESULT STDMETHODCALLTYPE proxy_IDirectInput_EnumDevicesBySemantics(IDirectInput8A *self, LPCSTR ptszUserName, LPDIACTIONFORMATA lpdiActionFormat, LPDIENUMDEVICESBYSEMANTICSCBA lpCallback, LPVOID pvRef, DWORD dwFlags);
HRESULT STDMETHODCALLTYPE proxy_IDirectInput_ConfigureDevices(IDirectInput8A *self, LPDICONFIGUREDEVICESCALLBACK lpdiCallback, LPDICONFIGUREDEVICESPARAMSA lpdiCDParams, DWORD dwFlags, LPVOID pvRefData);

HRESULT STDMETHODCALLTYPE proxy_IDirectInputDevice_QueryInterface(IDirectInputDevice8A *self, REFIID riid, LPVOID *ppvObj);
ULONG   STDMETHODCALLTYPE proxy_IDirectInputDevice_AddRef(IDirectInputDevice8A *self);
ULONG   STDMETHODCALLTYPE proxy_IDirectInputDevice_Release(IDirectInputDevice8A *self);
HRESULT STDMETHODCALLTYPE proxy_IDirectInputDevice_GetCapabilities(IDirectInputDevice8A *self, LPDIDEVCAPS lpDIDevCaps);
HRESULT STDMETHODCALLTYPE proxy_IDirectInputDevice_EnumObjects(IDirectInputDevice8A *self, LPDIENUMDEVICEOBJECTSCALLBACKA lpCallback, LPVOID pvRef, DWORD dwFlags);
HRESULT STDMETHODCALLTYPE proxy_IDirectInputDevice_GetProperty(IDirectInputDevice8A *self, REFGUID rguidProp, LPDIPROPHEADER pdiph);
HRESULT STDMETHODCALLTYPE proxy_IDirectInputDevice_SetProperty(IDirectInputDevice8A *self, REFGUID rguidProp, LPCDIPROPHEADER pdiph);
HRESULT STDMETHODCALLTYPE proxy_IDirectInputDevice_Acquire(IDirectInputDevice8A *self);
HRESULT STDMETHODCALLTYPE proxy_IDirectInputDevice_Unacquire(IDirectInputDevice8A *self);
HRESULT STDMETHODCALLTYPE proxy_IDirectInputDevice_GetDeviceState(IDirectInputDevice8A *self, DWORD cbData, LPVOID lpvData);
HRESULT STDMETHODCALLTYPE proxy_IDirectInputDevice_GetDeviceData(IDirectInputDevice8A *self, DWORD cbObjectData, LPDIDEVICEOBJECTDATA rgdod, LPDWORD pdwInOut, DWORD dwFlags);
HRESULT STDMETHODCALLTYPE proxy_IDirectInputDevice_SetDataFormat(IDirectInputDevice8A *self, LPCDIDATAFORMAT lpdf);
HRESULT STDMETHODCALLTYPE proxy_IDirectInputDevice_SetEventNotification(IDirectInputDevice8A *self, HANDLE hEvent);
HRESULT STDMETHODCALLTYPE proxy_IDirectInputDevice_SetCooperativeLevel(IDirectInputDevice8A *self, HWND hwnd, DWORD dwFlags);
HRESULT STDMETHODCALLTYPE proxy_IDirectInputDevice_GetObjectInfo(IDirectInputDevice8A *self, LPDIDEVICEOBJECTINSTANCEA pdidoi, DWORD dwObj, DWORD dwHow);
HRESULT STDMETHODCALLTYPE proxy_IDirectInputDevice_GetDeviceInfo(IDirectInputDevice8A *self, LPDIDEVICEINSTANCEA pdidi);
HRESULT STDMETHODCALLTYPE proxy_IDirectInputDevice_RunControlPanel(IDirectInputDevice8A *self, HWND hwndOwner, DWORD dwFlags);
HRESULT STDMETHODCALLTYPE proxy_IDirectInputDevice_Initialize(IDirectInputDevice8A *self, HINSTANCE hinst, DWORD dwVersion, REFGUID rguid);
HRESULT STDMETHODCALLTYPE proxy_IDirectInputDevice_CreateEffect(IDirectInputDevice8A *self, REFGUID rguid, LPCDIEFFECT lpeff, LPDIRECTINPUTEFFECT *ppdeff, LPUNKNOWN punkOuter);
HRESULT STDMETHODCALLTYPE proxy_IDirectInputDevice_EnumEffects(IDirectInputDevice8A *self, LPDIENUMEFFECTSCALLBACKA lpCallback, LPVOID pvRef, DWORD dwEffType);
HRESULT STDMETHODCALLTYPE proxy_IDirectInputDevice_GetEffectInfo(IDirectInputDevice8A *self, LPDIEFFECTINFOA pdei, REFGUID rguid);
HRESULT STDMETHODCALLTYPE proxy_IDirectInputDevice_GetForceFeedbackState(IDirectInputDevice8A *self, LPDWORD pdwOut);
HRESULT STDMETHODCALLTYPE proxy_IDirectInputDevice_SendForceFeedbackCommand(IDirectInputDevice8A *self, DWORD dwFlags);
HRESULT STDMETHODCALLTYPE proxy_IDirectInputDevice_EnumCreatedEffectObjects(IDirectInputDevice8A *self, LPDIENUMCREATEDEFFECTOBJECTSCALLBACK lpCallback, LPVOID pvRef, DWORD fl);
HRESULT STDMETHODCALLTYPE proxy_IDirectInputDevice_Escape(IDirectInputDevice8A *self, LPDIEFFESCAPE pesc);
HRESULT STDMETHODCALLTYPE proxy_IDirectInputDevice_Poll(IDirectInputDevice8A *self);
HRESULT STDMETHODCALLTYPE proxy_IDirectInputDevice_SendDeviceData(IDirectInputDevice8A *self, DWORD cbObjectData, LPCDIDEVICEOBJECTDATA rgdod, LPDWORD pdwInOut, DWORD fl);
HRESULT STDMETHODCALLTYPE proxy_IDirectInputDevice_EnumEffectsInFile(IDirectInputDevice8A *self, LPCSTR lpszFileName, LPDIENUMEFFECTSINFILECALLBACK pec, LPVOID pvRef, DWORD dwFlags);
HRESULT STDMETHODCALLTYPE proxy_IDirectInputDevice_WriteEffectToFile(IDirectInputDevice8A *self, LPCSTR lpszFileName, DWORD dwEntries, LPDIFILEEFFECT rgDiFileEft, DWORD dwFlags);
HRESULT STDMETHODCALLTYPE proxy_IDirectInputDevice_BuildActionMap(IDirectInputDevice8A *self, LPDIACTIONFORMATA lpdiaf, LPCSTR lpszUserName, DWORD dwFlags);
HRESULT STDMETHODCALLTYPE proxy_IDirectInputDevice_SetActionMap(IDirectInputDevice8A *self, LPDIACTIONFORMATA lpdiaf, LPCSTR lpszUserName, DWORD dwFlags);
HRESULT STDMETHODCALLTYPE proxy_IDirectInputDevice_GetImageInfo(IDirectInputDevice8A *self, LPDIDEVICEIMAGEINFOHEADERA lpdiDevImageInfoHeader);

#endif