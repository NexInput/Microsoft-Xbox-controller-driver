#include <atlstr.h> 
#include "NexInput.h"
#include "XInput.h"

#define DLLEXPORT extern "C" __declspec(dllexport)


_XInputGetState XboxControllerGetState;
_XInputSetState XboxControllerSetState;
_XInputGetBatteryInformation XboxControllerGetBatteryInformation;
_XINPUT_STATE XboxControllerPState;
XINPUT_VIBRATION XboxControllerVibration;
XINPUT_BATTERY_INFORMATION XboxControllerBatteryInformation;

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

			XboxControllerGetState = (_XInputGetState)GetProcAddress(hDll, "XInputGetState");
			XboxControllerSetState = (_XInputSetState)GetProcAddress(hDll, "XInputSetState");
			XboxControllerGetBatteryInformation = (_XInputGetBatteryInformation)GetProcAddress(hDll, "XInputGetBatteryInformation");

			if (XboxControllerGetState == NULL || XboxControllerSetState == NULL)
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

	DWORD myStatus = ERROR_DEVICE_NOT_CONNECTED;

	if (hDll != NULL)
		myStatus = XboxControllerGetState(dwUserIndex, &XboxControllerPState);

	if (myStatus == ERROR_SUCCESS) {
		pState->Buttons = XboxControllerPState.Gamepad.wButtons;
		pState->LeftTrigger = XboxControllerPState.Gamepad.bLeftTrigger;
		pState->RightTrigger = XboxControllerPState.Gamepad.bRightTrigger;
		pState->AxisLX = XboxControllerPState.Gamepad.sThumbLX;
		pState->AxisLY = XboxControllerPState.Gamepad.sThumbLY;
		pState->AxisRX = XboxControllerPState.Gamepad.sThumbRX;
		pState->AxisRY = XboxControllerPState.Gamepad.sThumbRY;
	}

	return myStatus;
}

DLLEXPORT DWORD __stdcall NEXInputSetState(__in DWORD dwUserIndex, __in NEX_OUTPUT_STATE *pOutputState)
{
	DWORD myStatus = ERROR_DEVICE_NOT_CONNECTED;

	if (hDll != NULL) {
		XboxControllerVibration.wLeftMotorSpeed = pOutputState->LeftMotorSpeed;
		XboxControllerVibration.wRightMotorSpeed = pOutputState->RightMotorSpeed;
		myStatus = XboxControllerSetState(dwUserIndex, &XboxControllerVibration);
	}

	return myStatus;
}

DLLEXPORT DWORD __stdcall NEXInputGetInfo(__in DWORD dwUserIndex, __out NEX_CONTROLLER_INFO *pControllerInfo)
{
	pControllerInfo->ControllerType = MICROSOFT_XBOX_CONTROLLER;
	pControllerInfo->ConnectType = NEX_CONTROLLER_WIRED;
	pControllerInfo->BatteryLevel = NEX_BATTERY_NONE;

	pControllerInfo->SupportRotation = false;

	DWORD myStatus = ERROR_DEVICE_NOT_CONNECTED;

	if (hDll != NULL)
		myStatus = XboxControllerGetBatteryInformation(dwUserIndex, 0, &XboxControllerBatteryInformation);

	if (myStatus == ERROR_SUCCESS)
		if (XboxControllerBatteryInformation.BatteryType != 0 && XboxControllerBatteryInformation.BatteryType != 1) //BATTERY_LEVEL_EMPTY, BATTERY_TYPE_WIRED
		{
			pControllerInfo->ConnectType = NEX_CONTROLLER_WIRELESS;
			switch (XboxControllerBatteryInformation.BatteryLevel)
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
