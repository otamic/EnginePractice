#pragma once
#include "engine_test.h"
#include "input_test.h"
#include "object_test.h"
#include <ddraw.h>

#define ddraw_init_struct(ddstruct) { memset(&ddstruct,0,sizeof(ddstruct)); ddstruct.dwSize=sizeof(ddstruct); }

class render_test : public engine_test
{
public:
	render_test();
	~render_test();

	int game_init();
	int game_main();
	int game_shutdown();

	virtual void render_main() = 0;
	virtual void logic_main() = 0;

	input_test * input;
protected:
	UCHAR * primary_buffer;
	UCHAR * back_buffer;
	int primary_lpitch;
	int back_lpitch;

	int clip_line(int &x1, int &y1, int &x2, int &y2);
	void draw_line(int x0, int y0, int x1, int y1, int color);
	void draw_clip_line(int x0, int y0, int x1, int y1, int color);
	void draw_object_4d_v1(const object_4d_v1 & obj);
	void draw_render_list_4d_v1(const render_list_4d_v1 & list);
private:
	LPDIRECTDRAW7 lpdd;
	LPDIRECTDRAWSURFACE7 lpdds_primary;
	LPDIRECTDRAWSURFACE7 lpdds_back;
	LPDIRECTDRAWCLIPPER  lpdd_clipper;
	LPDIRECTDRAWCLIPPER  lpdd_clipper_win;
	
	DDSURFACEDESC2 ddsd;
	DDBLTFX ddbltfx;
	DDSCAPS2 ddscaps;
	HRESULT ddrval;

	int window_client_x0;
	int window_client_y0;

	int min_clip_x, max_clip_x;
	int min_clip_y, max_clip_y;

	DWORD start_clock_count;

	int create();
	LPDIRECTDRAWSURFACE7 ddraw_create_surface(int width, int height, int mem_flags, USHORT color_key_value);
	LPDIRECTDRAWCLIPPER ddraw_attch_clipper(LPDIRECTDRAWSURFACE7 lpdds, int num_rects, LPRECT clip_list);
	UCHAR * ddraw_lock_back_surface();
	int ddraw_unlock_back_surface();
	int ddraw_flip();
	static int ddraw_fill_surface(LPDIRECTDRAWSURFACE7 lpdds, USHORT color, RECT * client = NULL);
};

