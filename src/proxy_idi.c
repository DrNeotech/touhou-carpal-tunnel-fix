#include "dinputproxy.h"

#define PDI(self)  ((proxy_IDirectInput *)(self))
#define REAL(self) (PDI(self)->di)

HRESULT proxy_IDirectInput_Create(IDirectInput8A *real, proxy_IDirectInput **out)
{
	proxy_IDirectInput *proxydi = (proxy_IDirectInput *)calloc(1, sizeof(*proxydi));
	if (!proxydi)
		return E_OUTOFMEMORY;

	proxydi->vtbl.QueryInterface        = proxy_IDirectInput_QueryInterface;
	proxydi->vtbl.AddRef                = proxy_IDirectInput_AddRef;
	proxydi->vtbl.Release               = proxy_IDirectInput_Release;
	proxydi->vtbl.CreateDevice          = proxy_IDirectInput_CreateDevice;
	proxydi->vtbl.EnumDevices           = proxy_IDirectInput_EnumDevices;
	proxydi->vtbl.GetDeviceStatus       = proxy_IDirectInput_GetDeviceStatus;
	proxydi->vtbl.RunControlPanel       = proxy_IDirectInput_RunControlPanel;
	proxydi->vtbl.Initialize            = proxy_IDirectInput_Initialize;
	proxydi->vtbl.FindDevice            = proxy_IDirectInput_FindDevice;
	proxydi->vtbl.EnumDevicesBySemantics = proxy_IDirectInput_EnumDevicesBySemantics;
	proxydi->vtbl.ConfigureDevices      = proxy_IDirectInput_ConfigureDevices;
	proxydi->lpVtbl = &proxydi->vtbl;
	proxydi->di = real;
	proxydi->refcount = 1;

	*out = proxydi;
	return DI_OK;
}

HRESULT STDMETHODCALLTYPE proxy_IDirectInput_QueryInterface(IDirectInput8A *self, REFIID riid, LPVOID *ppvObj)
{
	if (!ppvObj)
		return E_POINTER;

	if (IsEqualIID(riid, &IID_IUnknown) || IsEqualIID(riid, &IID_IDirectInput8A))
	{
		*ppvObj = self;
		proxy_IDirectInput_AddRef(self);
		return S_OK;
	}

	return REAL(self)->lpVtbl->QueryInterface(REAL(self), riid, ppvObj);
}

ULONG STDMETHODCALLTYPE proxy_IDirectInput_AddRef(IDirectInput8A *self)
{
	return (ULONG)InterlockedIncrement(&PDI(self)->refcount);
}

ULONG STDMETHODCALLTYPE proxy_IDirectInput_Release(IDirectInput8A *self)
{
	proxy_IDirectInput *proxydi = PDI(self);
	LONG ref = InterlockedDecrement(&proxydi->refcount);

	if (ref == 0)
	{
		proxydi->di->lpVtbl->Release(proxydi->di);
		free(proxydi);
	}

	return (ULONG)ref;
}

HRESULT STDMETHODCALLTYPE proxy_IDirectInput_CreateDevice(IDirectInput8A *self, REFGUID rguid, LPDIRECTINPUTDEVICE8A *lplpDevice, LPUNKNOWN pUnkOuter)
{
	IDirectInputDevice8A *did = NULL;
	proxy_IDirectInputDevice *proxydid;
	HRESULT ret;

	ret = REAL(self)->lpVtbl->CreateDevice(REAL(self), rguid, &did, pUnkOuter);
	if (FAILED(ret))
		return ret;

	proxydid = (proxy_IDirectInputDevice *)calloc(1, sizeof(*proxydid));
	if (!proxydid)
	{
		did->lpVtbl->Release(did);
		return E_OUTOFMEMORY;
	}

	proxydid->vtbl.QueryInterface         = proxy_IDirectInputDevice_QueryInterface;
	proxydid->vtbl.AddRef                 = proxy_IDirectInputDevice_AddRef;
	proxydid->vtbl.Release                = proxy_IDirectInputDevice_Release;
	proxydid->vtbl.GetCapabilities        = proxy_IDirectInputDevice_GetCapabilities;
	proxydid->vtbl.EnumObjects            = proxy_IDirectInputDevice_EnumObjects;
	proxydid->vtbl.GetProperty            = proxy_IDirectInputDevice_GetProperty;
	proxydid->vtbl.SetProperty            = proxy_IDirectInputDevice_SetProperty;
	proxydid->vtbl.Acquire                = proxy_IDirectInputDevice_Acquire;
	proxydid->vtbl.Unacquire              = proxy_IDirectInputDevice_Unacquire;
	proxydid->vtbl.GetDeviceState         = proxy_IDirectInputDevice_GetDeviceState;
	proxydid->vtbl.GetDeviceData          = proxy_IDirectInputDevice_GetDeviceData;
	proxydid->vtbl.SetDataFormat          = proxy_IDirectInputDevice_SetDataFormat;
	proxydid->vtbl.SetEventNotification   = proxy_IDirectInputDevice_SetEventNotification;
	proxydid->vtbl.SetCooperativeLevel    = proxy_IDirectInputDevice_SetCooperativeLevel;
	proxydid->vtbl.GetObjectInfo          = proxy_IDirectInputDevice_GetObjectInfo;
	proxydid->vtbl.GetDeviceInfo          = proxy_IDirectInputDevice_GetDeviceInfo;
	proxydid->vtbl.RunControlPanel        = proxy_IDirectInputDevice_RunControlPanel;
	proxydid->vtbl.Initialize             = proxy_IDirectInputDevice_Initialize;
	proxydid->vtbl.CreateEffect           = proxy_IDirectInputDevice_CreateEffect;
	proxydid->vtbl.EnumEffects            = proxy_IDirectInputDevice_EnumEffects;
	proxydid->vtbl.GetEffectInfo          = proxy_IDirectInputDevice_GetEffectInfo;
	proxydid->vtbl.GetForceFeedbackState  = proxy_IDirectInputDevice_GetForceFeedbackState;
	proxydid->vtbl.SendForceFeedbackCommand = proxy_IDirectInputDevice_SendForceFeedbackCommand;
	proxydid->vtbl.EnumCreatedEffectObjects = proxy_IDirectInputDevice_EnumCreatedEffectObjects;
	proxydid->vtbl.Escape                 = proxy_IDirectInputDevice_Escape;
	proxydid->vtbl.Poll                   = proxy_IDirectInputDevice_Poll;
	proxydid->vtbl.SendDeviceData         = proxy_IDirectInputDevice_SendDeviceData;
	proxydid->vtbl.EnumEffectsInFile      = proxy_IDirectInputDevice_EnumEffectsInFile;
	proxydid->vtbl.WriteEffectToFile      = proxy_IDirectInputDevice_WriteEffectToFile;
	proxydid->vtbl.BuildActionMap         = proxy_IDirectInputDevice_BuildActionMap;
	proxydid->vtbl.SetActionMap           = proxy_IDirectInputDevice_SetActionMap;
	proxydid->vtbl.GetImageInfo           = proxy_IDirectInputDevice_GetImageInfo;
	proxydid->lpVtbl = &proxydid->vtbl;
	proxydid->did = did;
	proxydid->refcount = 1;
	proxydid->sequence = 0;

	if (IsEqualGUID(rguid, &GUID_SysKeyboard))
		proxydid->bKeyboard = TRUE;
	else if (IsEqualGUID(rguid, &GUID_SysMouse))
		proxydid->bMouse = TRUE;

	*lplpDevice = (LPDIRECTINPUTDEVICE8A)proxydid;
	return ret;
}

HRESULT STDMETHODCALLTYPE proxy_IDirectInput_EnumDevices(IDirectInput8A *self, DWORD dwDevType, LPDIENUMDEVICESCALLBACKA lpCallback, LPVOID pvRef, DWORD dwFlags)
{
	return REAL(self)->lpVtbl->EnumDevices(REAL(self), dwDevType, lpCallback, pvRef, dwFlags);
}

HRESULT STDMETHODCALLTYPE proxy_IDirectInput_GetDeviceStatus(IDirectInput8A *self, REFGUID rguidInstance)
{
	return REAL(self)->lpVtbl->GetDeviceStatus(REAL(self), rguidInstance);
}

HRESULT STDMETHODCALLTYPE proxy_IDirectInput_RunControlPanel(IDirectInput8A *self, HWND hwndOwner, DWORD dwFlags)
{
	return REAL(self)->lpVtbl->RunControlPanel(REAL(self), hwndOwner, dwFlags);
}

HRESULT STDMETHODCALLTYPE proxy_IDirectInput_Initialize(IDirectInput8A *self, HINSTANCE hinst, DWORD dwVersion)
{
	return REAL(self)->lpVtbl->Initialize(REAL(self), hinst, dwVersion);
}

HRESULT STDMETHODCALLTYPE proxy_IDirectInput_FindDevice(IDirectInput8A *self, REFGUID rguid, LPCSTR pszName, LPGUID pguidInstance)
{
	return REAL(self)->lpVtbl->FindDevice(REAL(self), rguid, pszName, pguidInstance);
}

HRESULT STDMETHODCALLTYPE proxy_IDirectInput_EnumDevicesBySemantics(IDirectInput8A *self, LPCSTR ptszUserName, LPDIACTIONFORMATA lpdiActionFormat, LPDIENUMDEVICESBYSEMANTICSCBA lpCallback, LPVOID pvRef, DWORD dwFlags)
{
	return REAL(self)->lpVtbl->EnumDevicesBySemantics(REAL(self), ptszUserName, lpdiActionFormat, lpCallback, pvRef, dwFlags);
}

HRESULT STDMETHODCALLTYPE proxy_IDirectInput_ConfigureDevices(IDirectInput8A *self, LPDICONFIGUREDEVICESCALLBACK lpdiCallback, LPDICONFIGUREDEVICESPARAMSA lpdiCDParams, DWORD dwFlags, LPVOID pvRefData)
{
	return REAL(self)->lpVtbl->ConfigureDevices(REAL(self), lpdiCallback, lpdiCDParams, dwFlags, pvRefData);
}