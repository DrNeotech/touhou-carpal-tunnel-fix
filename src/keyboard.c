#include "dinputproxy.h"

#define NUM_KEYQUEUE_ITEMS  100000

static keyqueueitem_t keyqueue[NUM_KEYQUEUE_ITEMS];
static keyqueueitem_t *keyqueuehead = keyqueue;
static keyqueueitem_t *keyqueuetail = keyqueue;
static CRITICAL_SECTION keyqueue_lock;
static LONG keyqueue_lock_state = 0;

static void LockQueue(void)
{
	if (keyqueue_lock_state != 2)
	{
		if (InterlockedCompareExchange(&keyqueue_lock_state, 1, 0) == 0)
		{
			InitializeCriticalSection(&keyqueue_lock);
			InterlockedExchange(&keyqueue_lock_state, 2);
		}
		else
		{
			while (keyqueue_lock_state != 2)
				Sleep(0);
		}
	}
	EnterCriticalSection(&keyqueue_lock);
}

static void UnlockQueue(void)
{
	LeaveCriticalSection(&keyqueue_lock);
}

void SendBufferedKeys(proxy_IDirectInputDevice *proxydid, LPDIDEVICEOBJECTDATA rgdod, LPDWORD pdwInOut, DWORD dwCapacity)
{
	keyqueueitem_t key;
	DWORD x = *pdwInOut;

	while (x < dwCapacity && GetKeyFromQueue(&key))
	{
		rgdod[x].dwOfs = key.offset;
		rgdod[x].dwData = key.data;
		rgdod[x].dwTimeStamp = GetTickCount();
		rgdod[x].dwSequence = proxydid->sequence++;
		rgdod[x].uAppData = 0;
		x++;
	}

	*pdwInOut = x;
}

HRESULT KeyboardHandler(proxy_IDirectInputDevice *proxydid, LPDIDEVICEOBJECTDATA rgdod, LPDWORD pdwInOut, DWORD dwCapacity)
{
	SendBufferedKeys(proxydid, rgdod, pdwInOut, dwCapacity);
	return DI_OK;
}

void AddKeyToQueue(DWORD offset, DWORD data)
{
	LockQueue();
	if (keyqueuetail != keyqueue + NUM_KEYQUEUE_ITEMS)
	{
		keyqueuetail->offset = offset;
		keyqueuetail->data = data;
		keyqueuetail++;
	}
	UnlockQueue();
}

BOOL GetKeyFromQueue(keyqueueitem_t *key)
{
	BOOL got = FALSE;

	LockQueue();
	if (keyqueuetail > keyqueuehead)
	{
		*key = *keyqueuehead++;
		got = TRUE;

		if (keyqueuehead == keyqueuetail)
			keyqueuehead = keyqueuetail = keyqueue;
	}
	UnlockQueue();

	return got;
}

void PressKeyQueue(DWORD key)
{
	AddKeyToQueue(key, 0x80);
	AddKeyToQueue(key, 0x00);
}

void PressKeysString(const char *str)
{
	size_t xx;
	size_t len = strlen(str);

	for (xx = 0; xx < len; xx++)
	{
		switch(tolower((unsigned char)str[xx]))
		{
			case '1':
				PressKeyQueue(DIK_1);
				break;
			case '2':
				PressKeyQueue(DIK_2);
				break;
			case '3':
				PressKeyQueue(DIK_3);
				break;
			case '4':
				PressKeyQueue(DIK_4);
				break;
			case '5':
				PressKeyQueue(DIK_5);
				break;
			case '6':
				PressKeyQueue(DIK_6);
				break;
			case '7':
				PressKeyQueue(DIK_7);
				break;
			case '8':
				PressKeyQueue(DIK_8);
				break;
			case '9':
				PressKeyQueue(DIK_9);
				break;
			case '0':
				PressKeyQueue(DIK_0);
				break;
			case '-':
				PressKeyQueue(DIK_MINUS);
				break;
			case '_':
				PressKeyQueue(DIK_MINUS);
				break;
			case '=':
				PressKeyQueue(DIK_EQUALS);
				break;
			case '+':
				PressKeyQueue(DIK_EQUALS);
				break;
			case 'q':
				PressKeyQueue(DIK_Q);
				break;
			case 'w':
				PressKeyQueue(DIK_W);
				break;
			case 'e':
				PressKeyQueue(DIK_E);
				break;
			case 'r':
				PressKeyQueue(DIK_R);
				break;
			case 't':
				PressKeyQueue(DIK_T);
				break;
			case 'y':
				PressKeyQueue(DIK_Y);
				break;
			case 'u':
				PressKeyQueue(DIK_U);
				break;
			case 'i':
				PressKeyQueue(DIK_I);
				break;
			case 'o':
				PressKeyQueue(DIK_O);
				break;
			case 'p':
				PressKeyQueue(DIK_P);
				break;
			case 'a':
				PressKeyQueue(DIK_A);
				break;
			case 's':
				PressKeyQueue(DIK_S);
				break;
			case 'd':
				PressKeyQueue(DIK_D);
				break;
			case 'f':
				PressKeyQueue(DIK_F);
				break;
			case 'g':
				PressKeyQueue(DIK_G);
				break;
			case 'h':
				PressKeyQueue(DIK_H);
				break;
			case 'j':
				PressKeyQueue(DIK_J);
				break;
			case 'k':
				PressKeyQueue(DIK_K);
				break;
			case 'l':
				PressKeyQueue(DIK_L);
				break;
			case ';':
				PressKeyQueue(DIK_SEMICOLON);
				break;
			case ':':
				PressKeyQueue(DIK_SEMICOLON);
				break;
			case '\'':
				PressKeyQueue(DIK_APOSTROPHE);
				break;
			case '"':
				PressKeyQueue(DIK_APOSTROPHE);
				break;
			case '\\':
				PressKeyQueue(DIK_BACKSLASH);
				break;
			case '|':
				PressKeyQueue(DIK_BACKSLASH);
				break;
			case 'z':
				PressKeyQueue(DIK_Z);
				break;
			case 'x':
				PressKeyQueue(DIK_X);
				break;
			case 'c':
				PressKeyQueue(DIK_C);
				break;
			case 'v':
				PressKeyQueue(DIK_V);
				break;
			case 'b':
				PressKeyQueue(DIK_B);
				break;
			case 'n':
				PressKeyQueue(DIK_N);
				break;
			case 'm':
				PressKeyQueue(DIK_M);
				break;
			case '<':
				PressKeyQueue(DIK_COMMA);
				break;
			case ',':
				PressKeyQueue(DIK_COMMA);
				break;
			case '>':
				PressKeyQueue(DIK_PERIOD);
				break;
			case '.':
				PressKeyQueue(DIK_PERIOD);
				break;
			case '/':
				PressKeyQueue(DIK_SLASH);
				break;
			case '?':
				PressKeyQueue(DIK_SLASH);
				break;
			case ' ':
				PressKeyQueue(DIK_SPACE);
				break;
			case '\0':
				break;
			case '\n':
				PressKeyQueue(DIK_RETURN);
				break;
			default: 
				break;
		}
	}
}