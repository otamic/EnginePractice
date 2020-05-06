#pragma once
#include <Windows.h>
#include <windowsx.h>
#include <string>
using std::string;

class engine_test
{
public:
	engine_test();
	~engine_test();
	int create();
	int run();
	virtual int game_init() = 0;
	virtual int game_main() = 0;
	virtual int game_shutdown() = 0;
protected:
	string window_class_name;
	string window_title;
	int window_width;
	int window_height;
	int window_bpp;
	bool windowed_app;

	int window_x0;
	int window_y0;
	char buffer[256];
	DWORD start_clock_count;

	HWND main_window_handle;
	HINSTANCE main_instance;
	
};

#define DECLARE_MAIN(engine_class) \
	engine_test * engine;    \
	HINSTANCE hinstance;     \
	HWND main_window_handle; \
	                         \
	int WINAPI WinMain(HINSTANCE hinst, HINSTANCE hprevinstance, LPSTR lpcmdline, int ncmdshow) { \
		hinstance = hinst;   \
		engine = new engine_class(); \
		engine->create();    \
		int res = engine->run(); \
		return res;          \
	}