#include "input_test.h"


input_test::input_test(HWND handle, HINSTANCE instance) {
	main_window_handle = handle;
	main_instance = instance;

	if (FAILED(DirectInput8Create(main_instance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void **)&lpdi, NULL)));
		// print error

	if (!dinput_init_mouse());
		// print error

	if (!dinput_init_keyboard());
		// print error
}


input_test::~input_test() {
	dinput_release_mouse();
	dinput_release_keyborad();
	if (lpdi)
		lpdi->Release();
}

int input_test::dinput_init_mouse() {
	if (lpdi->CreateDevice(GUID_SysMouse, &lpdi_mouse, NULL) != DI_OK)
		return 0;

	if (lpdi_mouse->SetCooperativeLevel(main_window_handle, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND) != DI_OK)
		return 0;

	if (lpdi_mouse->SetDataFormat(&c_dfDIMouse) != DI_OK)
		return 0;

	if (lpdi_mouse->Acquire() != DI_OK)
		return 0;

	return 1;
}

int input_test::dinput_init_keyboard() {
	if (lpdi->CreateDevice(GUID_SysKeyboard, &lpdi_key, NULL) != DI_OK)
		return 0;

	if (lpdi_key->SetCooperativeLevel(main_window_handle, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND) != DI_OK)
		return 0;

	if (lpdi_key->SetDataFormat(&c_dfDIKeyboard) != DI_OK)
		return 0;
		
	if (lpdi_key->Acquire() != DI_OK)
		return 0;

	return 1;
}

int input_test::dinput_read_mouse() {
	if (lpdi_mouse) {
		if (lpdi_mouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&mouse_state) != DI_OK)
			return 0;
	}
	else {
		memset(&mouse_state, 0, sizeof(mouse_state));
		return 0;
	}

	return 1;
}

int input_test::dinput_read_keyboard() {
	if (lpdi_key) {
		if (lpdi_key->GetDeviceState(256, (LPVOID)keyboard_state) != DI_OK)
			return 0;
	}
	else {
		memset(keyboard_state, 0, sizeof(keyboard_state));
		return 0;
	}

	return 1;
}

void input_test::dinput_release_mouse() {
	if (lpdi_mouse) {
		lpdi_mouse->Unacquire();
		lpdi_mouse->Release();
	}
}

void input_test::dinput_release_keyborad() {
	if (lpdi_key) {
		lpdi_key->Unacquire();
		lpdi_key->Release();
	}
}