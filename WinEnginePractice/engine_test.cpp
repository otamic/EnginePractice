#include "engine_test.h"

extern HINSTANCE hinstance;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

engine_test::engine_test(){
	window_class_name = "name";
	window_title = "title";
	window_width = 800;
	window_height = 600;
	window_bpp = 32;
	windowed_app = true;

	window_x0 = 0;
	window_y0 = 0;

	main_window_handle = NULL;
	main_instance = NULL;

	WNDCLASS winclass;	// this will hold the class we create
	HWND	 hwnd;		// generic window handle
	HDC      hdc;       // generic dc
	PAINTSTRUCT ps;     // generic paintstruct

	// first fill in the window class stucture
	winclass.style = CS_DBLCLKS | CS_OWNDC |
		CS_HREDRAW | CS_VREDRAW;
	winclass.lpfnWndProc = WindowProc;
	winclass.cbClsExtra = 0;
	winclass.cbWndExtra = 0;
	winclass.hInstance = hinstance;
	winclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	winclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	winclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	winclass.lpszMenuName = NULL;
	winclass.lpszClassName = window_class_name.c_str();

	// register the window class
	if (!RegisterClass(&winclass));
		//return(0); print error

	// create the window, note the test to see if WINDOWED_APP is
	// true to select the appropriate window flags
	if (!(hwnd = CreateWindow(window_class_name.c_str(), // class
		window_title.c_str(),	 // title
		(windowed_app ? (WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION) : (WS_POPUP | WS_VISIBLE)),
		0, 0,	   // x,y
		window_width,  // width
		window_height, // height
		NULL,	   // handle to parent 
		NULL,	   // handle to menu
		hinstance,// instance
		NULL)));	// creation parms
		// return(0); print error

	// save the window handle and instance in a global
	main_window_handle = hwnd;
	main_instance = hinstance;
}


engine_test::~engine_test() {
}

int engine_test::create() {
	

	// resize the window so that client is really width x height
	if (windowed_app)
	{
		// now resize the window, so the client area is the actual size requested
		// since there may be borders and controls if this is going to be a windowed app
		// if the app is not windowed then it won't matter
		RECT window_rect = { 0, 0, window_width - 1, window_height - 1 };

		// make the call to adjust window_rect
		AdjustWindowRectEx(&window_rect,
			GetWindowStyle(main_window_handle),
			GetMenu(main_window_handle) != NULL,
			GetWindowExStyle(main_window_handle));

		// save the global client offsets, they are needed in DDraw_Flip()
		window_x0 = -window_rect.left;
		window_y0 = -window_rect.top;

		// now resize the window with a call to MoveWindow()
		MoveWindow(main_window_handle,
			0, // x position
			0, // y position
			window_rect.right - window_rect.left, // width
			window_rect.bottom - window_rect.top, // height
			FALSE);

		// show the window, so there's no garbage on first render
		ShowWindow(main_window_handle, SW_SHOW);
	} // end if windowed

	return 1;
}

int engine_test::run() {
	MSG		 msg;		// generic message
	
	game_init();

	// disable CTRL-ALT_DEL, ALT_TAB, comment this line out 
	// if it causes your system to crash
	SystemParametersInfo(SPI_SCREENSAVERRUNNING, TRUE, NULL, 0);

	// enter main event loop
	while (1)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			// test if this is a quit
			if (msg.message == WM_QUIT)
				break;

			// translate any accelerator keys
			TranslateMessage(&msg);

			// send the message to the window proc
			DispatchMessage(&msg);
		} // end if

		// main game processing goes here
		start_clock_count = GetTickCount();
		game_main();
		while ((GetTickCount() - start_clock_count) < 30);
		
	} // end while

	// shutdown game and release all resources
	game_shutdown();

	// enable CTRL-ALT_DEL, ALT_TAB, comment this line out 
	// if it causes your system to crash
	SystemParametersInfo(SPI_SCREENSAVERRUNNING, FALSE, NULL, 0);

	// return to Windows like this
	return(msg.wParam);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	PAINTSTRUCT	ps;
	HDC			hdc;

	switch (msg) {

	case WM_CREATE:{
		// do initialization stuff here
		return(0);
	} break;

	case WM_PAINT: {
		// start painting
		hdc = BeginPaint(hwnd, &ps);

		// end painting
		EndPaint(hwnd, &ps);
		return(0);
	} break;

	case WM_DESTROY: {
		// kill the application			
		PostQuitMessage(0);
		return(0);
	} break;

	default:break;

	} // end switch

	// process any messages that we didn't take care of 
	return (DefWindowProc(hwnd, msg, wparam, lparam));

} // end WinProc


