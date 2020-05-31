//=================== Nex Input ===================
//========== https://github.com/NexInput ==========
//=================================================

#pragma once

#define NEX_GAMEPAD_DPAD_UP             0x0001
#define NEX_GAMEPAD_DPAD_DOWN           0x0002
#define NEX_GAMEPAD_DPAD_LEFT           0x0004
#define NEX_GAMEPAD_DPAD_RIGHT          0x0008
#define NEX_GAMEPAD_START               0x0010
#define NEX_GAMEPAD_BACK                0x0020
#define NEX_GAMEPAD_LEFT_THUMB          0x0040
#define NEX_GAMEPAD_RIGHT_THUMB         0x0080
#define NEX_GAMEPAD_LEFT_SHOULDER       0x0100
#define NEX_GAMEPAD_RIGHT_SHOULDER      0x0200
#define NEX_GAMEPAD_A                   0x1000
#define NEX_GAMEPAD_B                   0x2000
#define NEX_GAMEPAD_X                   0x4000
#define NEX_GAMEPAD_Y                   0x8000

#define NEX_CONTROLLER_WIRED            0
#define NEX_CONTROLLER_WIRELESS         1
#define NEX_BATTERY_NONE                0
#define NEX_BATTERY_LOW                 1
#define NEX_BATTERY_FULL                5

#define NEX_INPUT_MAX_COUNT             4

#define ERROR_DEVICE_NOT_CONNECTED      1
#define ERROR_SUCCESS                   0

#define NEX_UNKNOWN_CONTROLLER          0

#define MICROSOFT_XBOX_CONTROLLER       1
#define MICROSOFT_XBOX_360_CONTROLLER   2
#define MICROSOFT_XBOX_ONE_CONTROLLER   3

#define SONY_DUALSHOCK_3_CONTROLLER     26
#define SONY_DUALSHOCK_4_CONTROLLER     27
#define SONY_DUALSHOCK_5_CONTROLLER     28

#define NINTENDO_SWITCH_PRO_CONTROLLER  51


typedef struct _NEX_INPUT_STATE
{
	WORD								Buttons;
	BYTE								LeftTrigger;
	BYTE								RightTrigger;
	SHORT								AxisLX;
	SHORT								AxisLY;
	SHORT								AxisRX;
	SHORT								AxisRY;
	float								Yaw;
	float								Pitch;
	float								Roll;
} NEX_INPUT_STATE, *PNEX_INPUT_STATE;

typedef struct _NEX_OUTPUT_STATE
{
	WORD								LeftMotorSpeed;
	WORD								RightMotorSpeed;
	BYTE								LEDBrightness;
	BYTE								LEDRed;
	BYTE								LEDGreen;
	BYTE								LEDBlue;
} NEX_OUTPUT_STATE, *PNEX_OUTPUT_STATE;

typedef struct _NEX_CONTROLLER_INFO
{
	WORD								ControllerType;
	BYTE								ConnectType;
	BYTE								BatteryLevel;
	bool								SupportRotation;
} NEX_CONTROLLER_INFO, *PNEX_CONTROLLER_INFO;


typedef DWORD(__stdcall *_NEXInputGetState)(__in DWORD dwUserIndex, __out NEX_INPUT_STATE *pInputState);
typedef DWORD(__stdcall *_NEXInputSetState)(__in DWORD dwUserIndex, __in NEX_OUTPUT_STATE *pOutputState);
typedef DWORD(__stdcall *_NEXInputGetInfo)(__in DWORD dwUserIndex, __out NEX_CONTROLLER_INFO *pControllerInfo);
typedef DWORD(__stdcall *_NEXInputPowerOff)(__in DWORD dwUserIndex);