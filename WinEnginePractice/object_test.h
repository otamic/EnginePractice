#pragma once
#include "math_test.h"

const int render_list_4d_v1_max_polys = 1000;

enum trans_coord { local_only, trans_only, local_to_trans };

struct camera_4d_v1;

struct plane_3d {
	point_3d p;
	vector_3d n;
	plane_3d() = default;
	plane_3d(point_3d p, vector_3d n) :p(p), n(n) {}
};

struct poly_ex_typ_1 {
	point_3d v[3];
};

struct poly_ex_typ_2 {
	point_3d * vertex_list;
	int vertices[3];
};

struct poly_4d_v1 {
	static const int POLY_STATE_ACTIVE = 0x0001;
	static const int POLY_STATE_CLIPPED = 0x0002;
	static const int POLY_STATE_BACKFACE = 0x0004;
	static const int POLY_ATTR_2SIDED = 0x0001;

	int state;
	int attribute;
	int color;

	point_4d * vertex_list;
	int vertices[3];
};

struct polyf_4d_v1 {
	static const int POLY_STATE_ACTIVE = 0x0001;
	static const int POLY_STATE_CLIPPED = 0x0002;
	static const int POLY_STATE_BACKFACE = 0x0004;
	static const int POLY_ATTR_2SIDED = 0x0001;

	int state;
	int attribute;
	int color;

	point_4d vertex_list[3];
	point_4d t_vertex_list[3];
	polyf_4d_v1 * next;
	polyf_4d_v1 * prev;
};

struct render_list_4d_v1 {
	const static int MAX_POLYS = 32768;
	int state;
	int attribute;

	int num_of_polys;

	polyf_4d_v1 * poly_point[render_list_4d_v1_max_polys];
	polyf_4d_v1 poly_data[render_list_4d_v1_max_polys];

	void reset() { num_of_polys = 0; }
	void insert(const polyf_4d_v1 & poly);
	void transform(matrix_44 matrix_trans, trans_coord coord_select);
	void remove_backfaces(const camera_4d_v1 & cam);
};

struct object_4d_v1 {
	static const int OBJECT_STATE_ACTIVE = 0x0001;
	static const int OBJECT_STATE_VISIBLE = 0x0002;
	static const int OBJECT_STATE_CULLED = 0x0004;

	int id;
	char name[64];
	int state;
	int attribute;
	float average_radius;
	float max_radius;

	point_4d world_position;
	vector_4d direction;
	vector_4d local_x, local_y, local_z;

	int num_of_vertices;

	point_4d vertex_list_local[64];
	point_4d vertex_list_trans[64];

	int num_of_polys;
	poly_4d_v1 poly_list[128];

	void transform(matrix_44 matrix_trans, trans_coord coord_select, bool is_trans_basis);
	void remove_backfaces(const camera_4d_v1 & cam);
	void reset_state();
};

struct camera_4d_v1 {
	static enum cam_attri { ealur, uvn };
	static enum rot_seq { xyz, yxz, xzy, yzx, zyx, zxy };
	static enum uvn_mode { uvn_simple, uvn_spherical };
	static const int CULL_OBJECT_X_PLANE = 1;
	static const int CULL_OBJECT_Y_PLANE = 2;
	static const int CULL_OBJECT_Z_PLANE = 4;
	static const int CULL_OBJECT_XYZ_PLANE = CULL_OBJECT_X_PLANE | CULL_OBJECT_Y_PLANE | CULL_OBJECT_Z_PLANE;
	
	int state;
	cam_attri attribute;

	point_4d position;

	vector_4d direction;	// UVN camera model
	vector_4d u, v, n;
	point_4d target;

	float view_dist;
//	float view_dist_v;
	
	float fov;

	float near_clip_z;
	float far_clip_z;

	plane_3d rt_clip_plane;
	plane_3d lt_clip_plane;
	plane_3d tp_clip_plane;
	plane_3d bt_clip_plane;

	float viewplane_width;
	float viewplane_height;

	float viewport_width;
	float viewport_height;
	float viewport_center_x;
	float viewport_center_y;

	float aspect_ratio;

	matrix_44 mcam;
	matrix_44 mper;
	matrix_44 mscr;

	camera_4d_v1(cam_attri attr,
		point_4d cam_pos,
		vector_4d cam_dir,
		point_4d cam_target,
		float near_clip_z,
		float far_clip_z,
		float fov,
		float viewport_width,
		float viewport_height);
	void build_matrix_euler(rot_seq seq);
	void build_matrix_uvn(uvn_mode mode);
	int cull_object_4d_v1(object_4d_v1 & obj, int cull_flags);
};

void model_to_world(object_4d_v1 & obj, trans_coord coord_select = local_to_trans);
void model_to_world(render_list_4d_v1 & list, const point_4d & world_pos, trans_coord coord_select = local_to_trans);
void world_to_camera(object_4d_v1 & obj, const camera_4d_v1 & camera);
void world_to_camera(render_list_4d_v1 & list, const camera_4d_v1 & camera);
void camera_to_perspective(object_4d_v1 & obj, const camera_4d_v1 & camera);
void camera_to_perspective(render_list_4d_v1 & list, const camera_4d_v1 & camera);
void perspective_to_screen(object_4d_v1 & obj, const camera_4d_v1 & camera);
void perspective_to_screen(render_list_4d_v1 & list, const camera_4d_v1 & camera);

