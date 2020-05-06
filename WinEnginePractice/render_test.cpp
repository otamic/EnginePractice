#include "render_test.h"

render_test::render_test() {
	lpdd = NULL;
	lpdds_primary = NULL;
	lpdds_back = NULL;
	lpdd_clipper = NULL;
	lpdd_clipper_win = NULL;
	primary_buffer = NULL;
	back_buffer = NULL;

	start_clock_count = 0;
	window_client_x0 = 0;
	window_client_y0 = 0;

	min_clip_x = min_clip_y = 0;
	max_clip_x = window_width - 1;
	max_clip_y = window_height - 1;

	input = new input_test(main_window_handle, main_instance);
	create();
}


render_test::~render_test(){
	if (lpdd_clipper)
		lpdd_clipper->Release();

	if (lpdd_clipper_win)
		lpdd_clipper_win->Release();

	if (lpdds_back)
		lpdds_back->Release();

	if (lpdds_primary)
		lpdds_primary->Release();

	if (lpdd)
		lpdd->Release();
}

int render_test::game_init() {
	return 1;
}

int render_test::game_main(){

	logic_main();
	
	ddraw_fill_surface(lpdds_back, 0);
	ddraw_lock_back_surface();
	
	render_main();

	ddraw_unlock_back_surface();
	int res = ddraw_flip();
	
	/*
	ddraw_fill_surface(lpdds_primary, 0);

	if (primary_buffer)
		return 1;

	ddraw_init_struct(ddsd);
	lpdds_primary->Lock(NULL, &ddsd, DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR, NULL);

	primary_buffer = (UCHAR *)ddsd.lpSurface;
	primary_lpitch = ddsd.lPitch;

	for (int i = 0; i < window_width; i++)
		for (int j = 0; j < window_height; j++)
			((int *)primary_buffer)[i + j * (primary_lpitch >> 2)] = 0x0000ff00;

	if (!primary_buffer)
		return 0;

	lpdds_primary->Unlock(NULL);

	primary_buffer = NULL;
	primary_lpitch = 0;
	*/
	return 1;
}

int render_test::game_shutdown() {
	return 1;
}

int render_test::create() {
	if (FAILED(DirectDrawCreateEx(NULL, (void **)&lpdd, IID_IDirectDraw7, NULL)))
		return 0;

	if (windowed_app) {
		if (FAILED(lpdd->SetCooperativeLevel(main_window_handle, DDSCL_NORMAL)))
			return 0;
	}
	else {
		if (FAILED(lpdd->SetCooperativeLevel(main_window_handle,
			DDSCL_ALLOWMODEX | DDSCL_FULLSCREEN |
			DDSCL_EXCLUSIVE | DDSCL_ALLOWREBOOT | DDSCL_MULTITHREADED)))
			return 0;

		if (FAILED(lpdd->SetDisplayMode(window_width, window_height, window_bpp, 0, 0)))
			return 0;
	}

	ddraw_init_struct(ddsd);

	if (!windowed_app) {
		ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
		ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX;

		ddsd.dwBackBufferCount = 1;
	}
	else {
		ddsd.dwFlags = DDSD_CAPS;
		ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

		ddsd.dwBackBufferCount = 0;
	}

	lpdd->CreateSurface(&ddsd, &lpdds_primary, NULL);

	/*
	DDPIXELFORMAT ddpf;

	ddraw_init_struct(ddpf);

	lpdds_primary->GetPixelFormat(&ddpf);
	*/

	if (!windowed_app) {
		ddscaps.dwCaps = DDSCAPS_BACKBUFFER;

		if (FAILED(lpdds_primary->GetAttachedSurface(&ddscaps, &lpdds_back)))
			return 0;
	}
	else {
		lpdds_back = ddraw_create_surface(window_width, window_height, 0, DDSCAPS_SYSTEMMEMORY);
	}

	if (windowed_app) {
		// Has Problem
		ddraw_fill_surface(lpdds_back, 0);
	}
	else {
		ddraw_fill_surface(lpdds_primary, 0);
		ddraw_fill_surface(lpdds_back, 0);
	}

	RECT screen_rect = { 0, 0, window_width, window_height };
	lpdd_clipper = ddraw_attch_clipper(lpdds_back, 1, &screen_rect);

	if (windowed_app)
	{
		// set windowed clipper
		if (FAILED(lpdd->CreateClipper(0, &lpdd_clipper_win, NULL)))
			return(0);

		if (FAILED(lpdd_clipper_win->SetHWnd(0, main_window_handle)))
			return(0);

		if (FAILED(lpdds_primary->SetClipper(lpdd_clipper_win)))
			return(0);
	} // end if screen windowed

	// return success
	return(1);
}

LPDIRECTDRAWSURFACE7 render_test::ddraw_create_surface(int width, int height, int mem_flags, USHORT color_key_value) {
	DDSURFACEDESC2 ddsd;
	LPDIRECTDRAWSURFACE7 lpdds;

	ddraw_init_struct(ddsd);
	ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;

	ddsd.dwWidth = width;
	ddsd.dwHeight = height;

	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | mem_flags;

	if (FAILED(lpdd->CreateSurface(&ddsd, &lpdds, NULL)))
		return NULL;

	DDCOLORKEY color_key;
	color_key.dwColorSpaceLowValue = color_key_value;
	color_key.dwColorSpaceHighValue = color_key_value;

	lpdds->SetColorKey(DDCKEY_SRCBLT, &color_key);

	return lpdds;
}

int render_test::ddraw_fill_surface(LPDIRECTDRAWSURFACE7 lpdds, USHORT color, RECT * client) {
	DDBLTFX ddbltfx;

	ddraw_init_struct(ddbltfx);

	ddbltfx.dwFillColor = color;

	lpdds->Blt(client, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx);

	return 1;
}

LPDIRECTDRAWCLIPPER render_test::ddraw_attch_clipper(LPDIRECTDRAWSURFACE7 lpdds, int num_rects, LPRECT clip_list) {
	LPDIRECTDRAWCLIPPER lpddcliper;
	LPRGNDATA region_data;
	int index;

	if (FAILED(lpdd->CreateClipper(0, &lpddcliper, NULL)))
		return NULL;

	region_data = (LPRGNDATA)malloc(sizeof(RGNDATAHEADER) + num_rects * sizeof(RECT));

	memcpy(region_data->Buffer, clip_list, sizeof(RECT)*num_rects);

	region_data->rdh.dwSize = sizeof(RGNDATAHEADER);
	region_data->rdh.iType = RDH_RECTANGLES;
	region_data->rdh.nCount = num_rects;
	region_data->rdh.nRgnSize = num_rects*sizeof(RECT);

	region_data->rdh.rcBound.left = 64000;
	region_data->rdh.rcBound.top = 64000;
	region_data->rdh.rcBound.right = -64000;
	region_data->rdh.rcBound.bottom = -64000;

	for (index = 0; index<num_rects; index++)
	{
		// test if the next rectangle unioned with the current bound is larger
		if (clip_list[index].left < region_data->rdh.rcBound.left)
			region_data->rdh.rcBound.left = clip_list[index].left;

		if (clip_list[index].right > region_data->rdh.rcBound.right)
			region_data->rdh.rcBound.right = clip_list[index].right;

		if (clip_list[index].top < region_data->rdh.rcBound.top)
			region_data->rdh.rcBound.top = clip_list[index].top;

		if (clip_list[index].bottom > region_data->rdh.rcBound.bottom)
			region_data->rdh.rcBound.bottom = clip_list[index].bottom;

	}

	if (FAILED(lpddcliper->SetClipList(region_data, 0)))
	{
		// release memory and return error
		free(region_data);
		return(NULL);
	} // end if

	// now attach the clipper to the surface
	if (FAILED(lpdds->SetClipper(lpddcliper)))
	{
		// release memory and return error
		free(region_data);
		return(NULL);
	} // end if

	// all is well, so release memory and send back the pointer to the new clipper
	free(region_data);
	return lpddcliper;
}

UCHAR * render_test::ddraw_lock_back_surface() {
	if (back_buffer)
		return back_buffer;

	ddraw_init_struct(ddsd);
	lpdds_back->Lock(NULL, &ddsd, DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR, NULL);

	back_buffer = (UCHAR *)ddsd.lpSurface;
	back_lpitch = ddsd.lPitch;

	return back_buffer;
}

int render_test::ddraw_unlock_back_surface() {
	if (!back_buffer)
		return 0;

	lpdds_back->Unlock(NULL);

	back_buffer = NULL;
	back_lpitch = 0;

	return 1;
}

int render_test::ddraw_flip() {
	if (primary_buffer || back_buffer)
		return 0;

	if (!windowed_app)
		while (FAILED(lpdds_primary->Flip(NULL, DDFLIP_WAIT)));
	else {
		RECT dest_rect;
		GetWindowRect(main_window_handle, &dest_rect);

		dest_rect.left += window_x0;
		dest_rect.top += window_y0;

		dest_rect.right = dest_rect.left + window_width - 1;
		dest_rect.bottom = dest_rect.top + window_height - 1;

		// int res = lpdds_primary->Blt(&dest_rect, lpdds_back, NULL, DDBLT_WAIT, NULL);
		if (FAILED(lpdds_primary->Blt(&dest_rect, lpdds_back, NULL, DDBLT_WAIT, NULL)))
			return 0;
	}
	return 1;
}

int render_test::clip_line(int &x1, int &y1, int &x2, int &y2) {
#define CLIP_CODE_C  0x0000
#define CLIP_CODE_N  0x0008
#define CLIP_CODE_S  0x0004
#define CLIP_CODE_E  0x0002
#define CLIP_CODE_W  0x0001

#define CLIP_CODE_NE 0x000a
#define CLIP_CODE_SE 0x0006
#define CLIP_CODE_NW 0x0009 
#define CLIP_CODE_SW 0x0005

	int xc1 = x1,
		yc1 = y1,
		xc2 = x2,
		yc2 = y2;

	int p1_code = 0,
		p2_code = 0;

	// determine codes for p1 and p2
	if (y1 < min_clip_y)
		p1_code |= CLIP_CODE_N;
	else
		if (y1 > max_clip_y)
			p1_code |= CLIP_CODE_S;

	if (x1 < min_clip_x)
		p1_code |= CLIP_CODE_W;
	else
		if (x1 > max_clip_x)
			p1_code |= CLIP_CODE_E;

	if (y2 < min_clip_y)
		p2_code |= CLIP_CODE_N;
	else
		if (y2 > max_clip_y)
			p2_code |= CLIP_CODE_S;

	if (x2 < min_clip_x)
		p2_code |= CLIP_CODE_W;
	else
		if (x2 > max_clip_x)
			p2_code |= CLIP_CODE_E;

	// try and trivially reject
	if ((p1_code & p2_code))
		return(0);

	// test for totally visible, if so leave points untouched
	if (p1_code == 0 && p2_code == 0)
		return(1);

	// determine end clip point for p1
	switch (p1_code)
	{
	case CLIP_CODE_C: break;

	case CLIP_CODE_N:
	{
		yc1 = min_clip_y;
		xc1 = x1 + 0.5 + (min_clip_y - y1)*(x2 - x1) / (y2 - y1);
	} break;
	case CLIP_CODE_S:
	{
		yc1 = max_clip_y;
		xc1 = x1 + 0.5 + (max_clip_y - y1)*(x2 - x1) / (y2 - y1);
	} break;

	case CLIP_CODE_W:
	{
		xc1 = min_clip_x;
		yc1 = y1 + 0.5 + (min_clip_x - x1)*(y2 - y1) / (x2 - x1);
	} break;

	case CLIP_CODE_E:
	{
		xc1 = max_clip_x;
		yc1 = y1 + 0.5 + (max_clip_x - x1)*(y2 - y1) / (x2 - x1);
	} break;

	// these cases are more complex, must compute 2 intersections
	case CLIP_CODE_NE:
	{
		// north hline intersection
		yc1 = min_clip_y;
		xc1 = x1 + 0.5 + (min_clip_y - y1)*(x2 - x1) / (y2 - y1);

		// test if intersection is valid, of so then done, else compute next
		if (xc1 < min_clip_x || xc1 > max_clip_x)
		{
			// east vline intersection
			xc1 = max_clip_x;
			yc1 = y1 + 0.5 + (max_clip_x - x1)*(y2 - y1) / (x2 - x1);
		} // end if

	} break;

	case CLIP_CODE_SE:
	{
		// south hline intersection
		yc1 = max_clip_y;
		xc1 = x1 + 0.5 + (max_clip_y - y1)*(x2 - x1) / (y2 - y1);

		// test if intersection is valid, of so then done, else compute next
		if (xc1 < min_clip_x || xc1 > max_clip_x)
		{
			// east vline intersection
			xc1 = max_clip_x;
			yc1 = y1 + 0.5 + (max_clip_x - x1)*(y2 - y1) / (x2 - x1);
		} // end if

	} break;

	case CLIP_CODE_NW:
	{
		// north hline intersection
		yc1 = min_clip_y;
		xc1 = x1 + 0.5 + (min_clip_y - y1)*(x2 - x1) / (y2 - y1);

		// test if intersection is valid, of so then done, else compute next
		if (xc1 < min_clip_x || xc1 > max_clip_x)
		{
			xc1 = min_clip_x;
			yc1 = y1 + 0.5 + (min_clip_x - x1)*(y2 - y1) / (x2 - x1);
		} // end if

	} break;

	case CLIP_CODE_SW:
	{
		// south hline intersection
		yc1 = max_clip_y;
		xc1 = x1 + 0.5 + (max_clip_y - y1)*(x2 - x1) / (y2 - y1);

		// test if intersection is valid, of so then done, else compute next
		if (xc1 < min_clip_x || xc1 > max_clip_x)
		{
			xc1 = min_clip_x;
			yc1 = y1 + 0.5 + (min_clip_x - x1)*(y2 - y1) / (x2 - x1);
		} // end if

	} break;

	default:break;

	} // end switch

	// determine clip point for p2
	switch (p2_code)
	{
	case CLIP_CODE_C: break;

	case CLIP_CODE_N:
	{
		yc2 = min_clip_y;
		xc2 = x2 + (min_clip_y - y2)*(x1 - x2) / (y1 - y2);
	} break;

	case CLIP_CODE_S:
	{
		yc2 = max_clip_y;
		xc2 = x2 + (max_clip_y - y2)*(x1 - x2) / (y1 - y2);
	} break;

	case CLIP_CODE_W:
	{
		xc2 = min_clip_x;
		yc2 = y2 + (min_clip_x - x2)*(y1 - y2) / (x1 - x2);
	} break;

	case CLIP_CODE_E:
	{
		xc2 = max_clip_x;
		yc2 = y2 + (max_clip_x - x2)*(y1 - y2) / (x1 - x2);
	} break;

	// these cases are more complex, must compute 2 intersections
	case CLIP_CODE_NE:
	{
		// north hline intersection
		yc2 = min_clip_y;
		xc2 = x2 + 0.5 + (min_clip_y - y2)*(x1 - x2) / (y1 - y2);

		// test if intersection is valid, of so then done, else compute next
		if (xc2 < min_clip_x || xc2 > max_clip_x)
		{
			// east vline intersection
			xc2 = max_clip_x;
			yc2 = y2 + 0.5 + (max_clip_x - x2)*(y1 - y2) / (x1 - x2);
		} // end if

	} break;

	case CLIP_CODE_SE:
	{
		// south hline intersection
		yc2 = max_clip_y;
		xc2 = x2 + 0.5 + (max_clip_y - y2)*(x1 - x2) / (y1 - y2);

		// test if intersection is valid, of so then done, else compute next
		if (xc2 < min_clip_x || xc2 > max_clip_x)
		{
			// east vline intersection
			xc2 = max_clip_x;
			yc2 = y2 + 0.5 + (max_clip_x - x2)*(y1 - y2) / (x1 - x2);
		} // end if

	} break;

	case CLIP_CODE_NW:
	{
		// north hline intersection
		yc2 = min_clip_y;
		xc2 = x2 + 0.5 + (min_clip_y - y2)*(x1 - x2) / (y1 - y2);

		// test if intersection is valid, of so then done, else compute next
		if (xc2 < min_clip_x || xc2 > max_clip_x)
		{
			xc2 = min_clip_x;
			yc2 = y2 + 0.5 + (min_clip_x - x2)*(y1 - y2) / (x1 - x2);
		} // end if

	} break;

	case CLIP_CODE_SW:
	{
		// south hline intersection
		yc2 = max_clip_y;
		xc2 = x2 + 0.5 + (max_clip_y - y2)*(x1 - x2) / (y1 - y2);

		// test if intersection is valid, of so then done, else compute next
		if (xc2 < min_clip_x || xc2 > max_clip_x)
		{
			xc2 = min_clip_x;
			yc2 = y2 + 0.5 + (min_clip_x - x2)*(y1 - y2) / (x1 - x2);
		} // end if

	} break;

	default:break;

	} // end switch

	// do bounds check
	if ((xc1 < min_clip_x) || (xc1 > max_clip_x) ||
		(yc1 < min_clip_y) || (yc1 > max_clip_y) ||
		(xc2 < min_clip_x) || (xc2 > max_clip_x) ||
		(yc2 < min_clip_y) || (yc2 > max_clip_y))
	{
		return(0);
	} // end if

	// store vars back
	x1 = xc1;
	y1 = yc1;
	x2 = xc2;
	y2 = yc2;

	return(1);

}

void render_test::draw_line(int x0, int y0, int x1, int y1, int color) {
	int dx,
		dy,
		dx2,
		dy2,
		x_inc,
		y_inc,
		error,
		index;

	int lpitch_2 = back_lpitch >> 2;
	int * vb_start = (int *)back_buffer + x0 + y0 * lpitch_2;
	dx = x1 - x0;
	dy = y1 - y0;

	if (dx >= 0) {
		x_inc = 1;
	}
	else {
		x_inc = -1;
		dx = -dx;
	}

	if (dy >= 0) {
		y_inc = lpitch_2;
	}
	else {
		y_inc = -lpitch_2;
		dy = -dy;
	}

	dx2 = dx << 1;
	dy2 = dy << 1;

	if (dx > dy) {
		error = dy2 - dx;
		for (index = 0; index <= dx; index++) {
			*vb_start = color;
			if (error >= 0) {
				error -= dx2;
				vb_start += y_inc;
			}
			error += dy2;
			vb_start += x_inc;
		}
	}
	else {
		error = dx2 - dy;
		for (index = 0; index <= dy; index++) {
			*vb_start = color;
			if (error >= 0) {
				error -= dy2;
				vb_start += x_inc;
			}
			error += dx2;
			vb_start += y_inc;
		}
	}
}

void render_test::draw_clip_line(int x0, int y0, int x1, int y1, int color) {
	int cxs = x0, cys = y0,
		cxe = x1, cye = y1;
	if (clip_line(cxs, cys, cxe, cye))
		draw_line(cxs, cys, cxe, cye, color);
}

void render_test::draw_object_4d_v1(const object_4d_v1 & obj){
	for (int poly = 0; poly < obj.num_of_polys; poly++) {
		if (!(obj.poly_list[poly].state & poly_4d_v1::POLY_STATE_ACTIVE) ||
			(obj.poly_list[poly].state & poly_4d_v1::POLY_STATE_CLIPPED) ||
			(obj.poly_list[poly].state & poly_4d_v1::POLY_STATE_BACKFACE))
			continue;

		int index_0 = obj.poly_list[poly].vertices[0];
		int index_1 = obj.poly_list[poly].vertices[1];
		int index_2 = obj.poly_list[poly].vertices[2];

		draw_clip_line(obj.vertex_list_trans[index_0].x,
			obj.vertex_list_trans[index_0].y,
			obj.vertex_list_trans[index_1].x,
			obj.vertex_list_trans[index_1].y,
			obj.poly_list[poly].color);

		draw_clip_line(obj.vertex_list_trans[index_1].x,
			obj.vertex_list_trans[index_1].y,
			obj.vertex_list_trans[index_2].x,
			obj.vertex_list_trans[index_2].y,
			obj.poly_list[poly].color);

		draw_clip_line(obj.vertex_list_trans[index_2].x,
			obj.vertex_list_trans[index_2].y,
			obj.vertex_list_trans[index_0].x,
			obj.vertex_list_trans[index_0].y,
			obj.poly_list[poly].color);
	}
}

void render_test::draw_render_list_4d_v1(const render_list_4d_v1 & list) {
	for (int poly = 0; poly < list.num_of_polys; poly++) {
		if (!(list.poly_point[poly]->state & polyf_4d_v1::POLY_STATE_ACTIVE) ||
			(list.poly_point[poly]->state & polyf_4d_v1::POLY_STATE_CLIPPED) ||
			(list.poly_point[poly]->state & polyf_4d_v1::POLY_STATE_BACKFACE))
			continue;

		draw_clip_line(list.poly_point[poly]->t_vertex_list[0].x,
			list.poly_point[poly]->t_vertex_list[0].y,
			list.poly_point[poly]->t_vertex_list[1].x,
			list.poly_point[poly]->t_vertex_list[1].y,
			list.poly_point[poly]->color);

		draw_clip_line(list.poly_point[poly]->t_vertex_list[1].x,
			list.poly_point[poly]->t_vertex_list[1].y,
			list.poly_point[poly]->t_vertex_list[2].x,
			list.poly_point[poly]->t_vertex_list[2].y,
			list.poly_point[poly]->color);

		draw_clip_line(list.poly_point[poly]->t_vertex_list[2].x,
			list.poly_point[poly]->t_vertex_list[2].y,
			list.poly_point[poly]->t_vertex_list[0].x,
			list.poly_point[poly]->t_vertex_list[0].y,
			list.poly_point[poly]->color);
	}
}