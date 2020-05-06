#define INITGUID

#define WIN32_LEAN_AND_MEAN

#include <windows.h>   
#include <windowsx.h> 
#include <mmsystem.h>
#include <iostream> 
#include <conio.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h> 
#include <math.h>
#include <io.h>
#include <fcntl.h>

#include <ddraw.h>  
#include <dsound.h>
#include <dmksctrl.h>
#include <dmusici.h>
#include <dmusicc.h>
#include <dmusicf.h>
#include <dinput.h>

#include "testlib.h"

#include "render_test.h"


class my_engine : public render_test {
	int x;
	int y;
	int color;
	
	void render_main() {
		
		/*
		x = (x + 1) % window_width;
		y = (y + 1) % window_height;
		((int *)back_buffer)[x + y * (back_lpitch >> 2)] = color;
		 */

		/*
		for (int i = 0; i < window_height; i++)
			for (int j = 0; j < window_width; j++)
				((int *)back_buffer)[j + i * (back_lpitch >> 2)] = color;
		*/

		((int *)back_buffer)[x + y * (back_lpitch >> 2)] = color;
	
		draw_line(100, 100, 200, 200, color);
		draw_clip_line(100, -100, 100, 1000, color);
	}

	void logic_main() {
		input->dinput_read_keyboard();
		if (input->keyboard_state[DIK_LEFT])
			x = (x - 1 + window_width) % window_width;
		if (input->keyboard_state[DIK_RIGHT])
			x = (x + 1 + window_width) % window_width;
		if (input->keyboard_state[DIK_UP])
			y = (y - 1 + window_height) % window_height;
		if (input->keyboard_state[DIK_DOWN])
			y = (y + 1 + window_height) % window_height;

		if (input->dinput_read_mouse()) {
			x = (x + input->mouse_state.lX + window_width) % window_width;
			y = (y + input->mouse_state.lY + window_height) % window_height;
		}

	}

public:
	my_engine() {
		x = 0;
		y = 0;
		color = 0x00ff0000; // red
	}
};


class my_test : public render_test {
private:
	camera_4d_v1 * cam;
	render_list_4d_v1 rend_list;
	polyf_4d_v1 poly;
	point_4d poly_pos;

	void logic_main() {
		static matrix_44 mrot;
		static float ang_y = 0.0;

		mrot = rotate_matrix(0.0, ang_y, 0.0);
		if (++ang_y >= 360.0) ang_y = 0.0;

		rend_list.reset();
		rend_list.insert(poly);

		input->dinput_read_keyboard();
		if (input->keyboard_state[DIK_LEFT])
			cam->position.x--;
		if (input->keyboard_state[DIK_RIGHT])
			cam->position.x++;
		if (input->keyboard_state[DIK_UP])
			cam->position.z++;
		if (input->keyboard_state[DIK_DOWN])
			cam->position.z--;

		if (input->dinput_read_mouse()) {
			cam->direction.x += input->mouse_state.lY;
			cam->direction.y += input->mouse_state.lX;
		}

		rend_list.transform(mrot, local_only);
		model_to_world(rend_list, poly_pos);
		cam->build_matrix_euler(camera_4d_v1::zyx);
		world_to_camera(rend_list, *cam);
		camera_to_perspective(rend_list, *cam);
		perspective_to_screen(rend_list, *cam);
	}

	void render_main() {
		draw_render_list_4d_v1(rend_list);
	}
public:
	my_test() {
		point_4d cam_pos = { 0, 0, -100 };
		vector_4d cam_dir = { 0, 0, 0 };
		vector_4d vscale = { .5, .5, .5 }, vpos = { 0, 0, 0 }, vrot = { 0, 0, 0 };

		poly_pos = { 0, 0, 100 };

		poly.state = polyf_4d_v1::POLY_STATE_ACTIVE;
		poly.attribute = 0;
		poly.color = 0x00ff0000; // red;

		poly.vertex_list[0].x = 0;
		poly.vertex_list[0].y = 50;
		poly.vertex_list[0].z = 0;
		poly.vertex_list[0].w = 1;

		poly.vertex_list[1].x = 50;
		poly.vertex_list[1].y = -50;
		poly.vertex_list[1].z = 0;
		poly.vertex_list[1].w = 1;

		poly.vertex_list[2].x = -50;
		poly.vertex_list[2].y = -50;
		poly.vertex_list[2].z = 0;
		poly.vertex_list[2].w = 1;

		poly.next = poly.prev = nullptr;

		cam = new camera_4d_v1(camera_4d_v1::ealur,
			cam_pos,
			cam_dir,
			cam_dir,
			50.0,
			500.0,
			90.0,
			window_width,
			window_height);
	}
};

DECLARE_MAIN(my_test)