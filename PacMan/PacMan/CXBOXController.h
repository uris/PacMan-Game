#ifndef _XBOX_CONTROLLER_H_
#define _XBOX_CONTROLLER_H_

// No MFC
#define WIN32_LEAN_AND_MEAN

// We need the Windows Header and the XInput Header
#include <windows.h>
#include <XInput.h>

// Now, the XInput Library
// NOTE: COMMENT THIS OUT IF YOU ARE NOT USING A COMPILER THAT SUPPORTS THIS METHOD OF LINKING LIBRARIES
#pragma comment(lib, "XInput.lib")

// XBOX Controller Class Definition
class CXBOXController
{
private:
	XINPUT_STATE _controllerState;
	int _controllerNum;
public:
	ULONGLONG lastPress = 0;
	ULONGLONG tick_interval = 300; // for preventing continous pressing where needed
	CXBOXController(int playerNumber);
	XINPUT_STATE GetState();
	bool IsConnected();
	bool NotIsRepeat();
	void Vibrate(int leftVal = 0, int rightVal = 0);
};

#endif

/*
    These are the buttons and controls of the xbox controller
    https://docs.microsoft.com/en-us/windows/win32/api/xinput/ns-xinput-xinput_gamepad?redirectedfrom=MSDN

    XINPUT_GAMEPAD_DPAD_UP          0x00000001
    XINPUT_GAMEPAD_DPAD_DOWN        0x00000002
    XINPUT_GAMEPAD_DPAD_LEFT        0x00000004
    XINPUT_GAMEPAD_DPAD_RIGHT       0x00000008
    XINPUT_GAMEPAD_START            0x00000010
    XINPUT_GAMEPAD_BACK             0x00000020
    XINPUT_GAMEPAD_LEFT_THUMB       0x00000040
    XINPUT_GAMEPAD_RIGHT_THUMB      0x00000080
    XINPUT_GAMEPAD_LEFT_SHOULDER    0x0100
    XINPUT_GAMEPAD_RIGHT_SHOULDER   0x0200
    XINPUT_GAMEPAD_A                0x1000
    XINPUT_GAMEPAD_B                0x2000
    XINPUT_GAMEPAD_X                0x4000
    XINPUT_GAMEPAD_Y                0x8000

    sThumbLX
    Left thumbstick x-axis value. Each of the thumbstick axis members is a signed value between -32768 and 32767 describing the position of the thumbstick. A value of 0 is centered. Negative values signify down or to the left. Positive values signify up or to the right. The constants XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE or XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE can be used as a positive and negative value to filter a thumbstick input.

    sThumbLY
    Left thumbstick y-axis value. The value is between -32768 and 32767.

    sThumbRX
    Right thumbstick x-axis value. The value is between -32768 and 32767.

    sThumbRY
    Right thumbstick y-axis value. The value is between -32768 and 32767.

    */