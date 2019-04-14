#include "stdafx.h"
#include <atlstr.h> 

#define DLLEXPORT extern "C" __declspec(dllexport)

#define NEX_GAMEPAD_DPAD_UP				0x0001
#define NEX_GAMEPAD_DPAD_DOWN			0x0002
#define NEX_GAMEPAD_DPAD_LEFT			0x0004
#define NEX_GAMEPAD_DPAD_RIGHT			0x0008
#define NEX_GAMEPAD_START				0x0010
#define NEX_GAMEPAD_BACK				0x0020
#define NEX_GAMEPAD_LEFT_THUMB			0x0040
#define NEX_GAMEPAD_RIGHT_THUMB			0x0080
#define NEX_GAMEPAD_LEFT_SHOULDER		0x0100
#define NEX_GAMEPAD_RIGHT_SHOULDER		0x0200
#define NEX_GAMEPAD_A					0x1000
#define NEX_GAMEPAD_B					0x2000
#define NEX_GAMEPAD_X					0x4000
#define NEX_GAMEPAD_Y					0x8000

#define NEX_CONTROLLER_WIRED			0
#define NEX_CONTROLLER_WIRELESS			1
#define NEX_BATTERY_NONE				0
#define NEX_BATTERY_LOW					1
#define NEX_BATTERY_FULL				5

#define NEX_INPUT_MAX_COUNT				4

#define ERROR_DEVICE_NOT_CONNECTED		1
#define ERROR_SUCCESS					0

typedef struct _NEX_INPUT_STATE
{
	WORD								Buttons;
	BYTE								LeftTrigger;
	BYTE								RightTrigger;
	SHORT								AxisLX;
	SHORT								AxisLY;
	SHORT								AxisRX;
	SHORT								AxisRY;
	bool								SupportRotation;
	float								Yaw;
	float								Pitch;
	float								Roll;
} NEX_INPUT_STATE, *PNEX_INPUT_STATE;

typedef struct _NEX_OUTPUT_STATE
{
	WORD								LeftMotorSpeed;
	WORD								RightMotorSpeed;
	bool								UseLed;
	BYTE								LEDRed;
	BYTE								LEDGreen;
	BYTE								LEDBlue;
} NEX_OUTPUT_STATE, *PNEX_OUTPUT_STATE;

#define NEX_UNKNOWN_CONTROLLER			0;

#define MICROSOFT_XBOX_360_CONTROLLER	1;
#define MICROSOFT_XBOX_ONE_CONTROLLER	2;

#define SONY_DUALSHOCK_3_CONTROLLER		26;
#define SONY_DUALSHOCK_4_CONTROLLER		27;

#define NINTENDO_SWITCH_PRO_CONTROLLER	51;

typedef struct _NEX_CONTROLLER_INFO
{
	WORD								ControllerType;
	BYTE								ConnectType;
	BYTE								BatteryLevel;
} NEX_CONTROLLER_INFO, *PNEX_CONTROLLER_INFO;

//Xbox 360 controller

typedef struct _XINPUT_GAMEPAD
{
	WORD                                wButtons;
	BYTE                                bLeftTrigger;
	BYTE                                bRightTrigger;
	SHORT                               sThumbLX;
	SHORT                               sThumbLY;
	SHORT                               sThumbRX;
	SHORT                               sThumbRY;
} XINPUT_GAMEPAD, *PXINPUT_GAMEPAD;

typedef struct _XINPUT_STATE
{
	DWORD                               dwPacketNumber;
	XINPUT_GAMEPAD                      Gamepad;
} XINPUT_STATE, *PXINPUT_STATE;

typedef struct _XINPUT_VIBRATION
{
	WORD                                wLeftMotorSpeed;
	WORD                                wRightMotorSpeed;
} XINPUT_VIBRATION, *PXINPUT_VIBRATION;

typedef struct _XINPUT_BATTERY_INFORMATION
{
	BYTE BatteryType;
	BYTE BatteryLevel;
} XINPUT_BATTERY_INFORMATION, *PXINPUT_BATTERY_INFORMATION;

typedef DWORD(__stdcall *_XInputGetState)(_In_ DWORD dwUserIndex, _Out_ XINPUT_STATE *pState);
typedef DWORD(__stdcall *_XInputSetState)(_In_ DWORD dwUserIndex, _In_ XINPUT_VIBRATION *pVibration);
typedef DWORD(__stdcall *_XInputGetBatteryInformation)(_In_ DWORD dwUserIndex, _In_ BYTE devType, _Out_ XINPUT_BATTERY_INFORMATION *pBatteryInformation);

_XInputGetState Xbox360ControllerGetState;
_XInputSetState Xbox360ControllerSetState;
_XInputGetBatteryInformation Xbox360ControllerGetBatteryInformation;
_XINPUT_STATE Xbox360ControllerPState;
XINPUT_VIBRATION Xbox360ControllerVibration;
XINPUT_BATTERY_INFORMATION Xbox360ControllerBatteryInformation;

HMODULE hDll;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH: {
		
		TCHAR XInputPath[MAX_PATH] = { 0 };
		GetSystemWindowsDirectory(XInputPath, sizeof(XInputPath));

		_tcscat_s(XInputPath, sizeof(XInputPath), _T("\\System32\\xinput1_3.dll")); //Separate paths for different architectures are not required. Windows does it by itself.

		hDll = LoadLibrary(XInputPath);

		if (hDll != NULL) {

			Xbox360ControllerGetState = (_XInputGetState)GetProcAddress(hDll, "XInputGetState");
			Xbox360ControllerSetState = (_XInputSetState)GetProcAddress(hDll, "XInputSetState");
			Xbox360ControllerGetBatteryInformation = (_XInputGetBatteryInformation)GetProcAddress(hDll, "XInputGetBatteryInformation");

			if (Xbox360ControllerGetState == NULL || Xbox360ControllerSetState == NULL)
				hDll = NULL;
		}

		break;
	}

	case DLL_PROCESS_DETACH: {
		if (hDll != NULL) {
			FreeLibrary(hDll);
			hDll = nullptr;
		}
		break;
	}
	}
	return true;
}

DLLEXPORT DWORD __stdcall NEXInputGetState(__in DWORD dwUserIndex, __out NEX_INPUT_STATE *pState)
{
	pState->Buttons = 0;
	pState->LeftTrigger = 0;
	pState->RightTrigger = 0;
	pState->AxisLX = 0;
	pState->AxisLY = 0;
	pState->AxisRX = 0;
	pState->AxisRY = 0;
	pState->Yaw = 0;
	pState->Pitch = 0;
	pState->Roll = 0;

	pState->SupportRotation = false;

	DWORD myStatus = ERROR_DEVICE_NOT_CONNECTED;

	if (hDll != NULL)
		myStatus = Xbox360ControllerGetState(dwUserIndex, &Xbox360ControllerPState);

	if (myStatus == ERROR_SUCCESS) {
		pState->Buttons = Xbox360ControllerPState.Gamepad.wButtons;
		pState->LeftTrigger = Xbox360ControllerPState.Gamepad.bLeftTrigger;
		pState->RightTrigger = Xbox360ControllerPState.Gamepad.bRightTrigger;
		pState->AxisLX = Xbox360ControllerPState.Gamepad.sThumbLX;
		pState->AxisLY = Xbox360ControllerPState.Gamepad.sThumbLY;
		pState->AxisRX = Xbox360ControllerPState.Gamepad.sThumbRX;
		pState->AxisRY = Xbox360ControllerPState.Gamepad.sThumbRY;
		pState->Yaw = 0;
		pState->Pitch = 0;
		pState->Roll = 0;
	}

	return myStatus;
}

DLLEXPORT DWORD __stdcall NEXInputSetState(__in DWORD dwUserIndex, __in NEX_OUTPUT_STATE *pOutputState)
{
	DWORD myStatus = ERROR_DEVICE_NOT_CONNECTED;

	if (hDll != NULL) {
		Xbox360ControllerVibration.wLeftMotorSpeed = pOutputState->LeftMotorSpeed;
		Xbox360ControllerVibration.wRightMotorSpeed = pOutputState->RightMotorSpeed;
		myStatus = Xbox360ControllerSetState(dwUserIndex, &Xbox360ControllerVibration);
	}

	return myStatus;
}

DLLEXPORT DWORD __stdcall NEXInputGetInfo(__in DWORD dwUserIndex, __out NEX_CONTROLLER_INFO *pControllerInfo)
{
	pControllerInfo->ControllerType = MICROSOFT_XBOX_360_CONTROLLER;
	pControllerInfo->ConnectType = NEX_CONTROLLER_WIRED;
	pControllerInfo->BatteryLevel = NEX_BATTERY_NONE;

	DWORD myStatus = ERROR_DEVICE_NOT_CONNECTED;

	if (hDll != NULL)
		myStatus = Xbox360ControllerGetBatteryInformation(dwUserIndex, 0, &Xbox360ControllerBatteryInformation);

	if (myStatus == ERROR_SUCCESS)
		if (Xbox360ControllerBatteryInformation.BatteryType != 0 && Xbox360ControllerBatteryInformation.BatteryType != 1) //BATTERY_LEVEL_EMPTY, BATTERY_TYPE_WIRED
		{
			pControllerInfo->ConnectType = NEX_CONTROLLER_WIRELESS;
			switch (Xbox360ControllerBatteryInformation.BatteryLevel)
			{
				case 1: {
					pControllerInfo->BatteryLevel = NEX_BATTERY_LOW;
					break;
				}
				case 2: {
					pControllerInfo->BatteryLevel = 3;
					break;
				}
				case 3: {
					pControllerInfo->BatteryLevel = NEX_BATTERY_FULL;
					break;
				}

			}
		}
	return myStatus;
}

DLLEXPORT DWORD __stdcall NEXInputPowerOff(__in DWORD dwUserIndex)
{
	return ERROR_SUCCESS;
}
