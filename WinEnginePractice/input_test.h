#pragma once
#include <dinput.h>

class input_test {
public:
	input_test(HWND handle, HINSTANCE instance);
	~input_test();

	int dinput_read_mouse();
	int dinput_read_keyboard();

	UCHAR keyboard_state[256];
	DIMOUSESTATE mouse_state;
	DIJOYSTATE joy_state;
	bool joystick_found;
private:
	HWND main_window_handle;
	HINSTANCE main_instance;

	LPDIRECTINPUT8 lpdi;
	LPDIRECTINPUTDEVICE8 lpdi_key;
	LPDIRECTINPUTDEVICE8 lpdi_mouse;
	LPDIRECTINPUTDEVICE8 lpdi_joy;
	GUID joystick_guid;
	char joy_name[80];

	int dinput_init_mouse();
	int dinput_init_keyboard();
	void dinput_release_mouse();
	void dinput_release_keyborad();
};

