#include "object_test.h"
#include <cmath>
#include <cstdlib>
#include <cstring>

void render_list_4d_v1::transform(matrix_44 matrix_trans, trans_coord coord_select) {
	switch (coord_select) {
	case local_only:
		for (int poly = 0; poly < num_of_polys; poly++) {
			polyf_4d_v1 * curr_poly = poly_point[poly];
			// conditions, have to be done
			for (int vertex = 0; vertex < 3; vertex++) {
			//	point_4d result = matrix_trans.mul_vector_4d(curr_poly->vertex_list[vertex]); // wrong !
				point_4d result = curr_poly->vertex_list[vertex].mul_matrix_44(matrix_trans);
				curr_poly->vertex_list[vertex] = result;
			}
		}
		break;
	case trans_only:
		for (int poly = 0; poly < num_of_polys; poly++) {
			polyf_4d_v1 * curr_poly = poly_point[poly];
			// conditions, have to be done
			for (int vertex = 0; vertex < 3; vertex++) {
			//	point_4d result = matrix_trans.mul_vector_4d(curr_poly->t_vertex_list[vertex]);
				point_4d result = curr_poly->t_vertex_list[vertex].mul_matrix_44(matrix_trans);
				curr_poly->t_vertex_list[vertex] = result;
			}
		}
		break;
	case local_to_trans:
		for (int poly = 0; poly < num_of_polys; poly++) {
			polyf_4d_v1 * curr_poly = poly_point[poly];
			// conditions, have to be done
			for (int vertex = 0; vertex < 3; vertex++) {
			//	point_4d result = matrix_trans.mul_vector_4d(curr_poly->vertex_list[vertex]);
				point_4d result = curr_poly->vertex_list[vertex].mul_matrix_44(matrix_trans);
				curr_poly->t_vertex_list[vertex] = result;
			}
		}
		break;
	default:
		break;
	}
}

void render_list_4d_v1::insert(const polyf_4d_v1 & poly) {
	if (num_of_polys >= MAX_POLYS)
		return;

	poly_point[num_of_polys] = & poly_data[num_of_polys];
	memcpy((void *)&poly_data[num_of_polys], (void *)&poly, sizeof(polyf_4d_v1));

	if (num_of_polys == 0) {
		poly_data[0].next = poly_data[0].prev = nullptr;
	}
	else {
		poly_data[num_of_polys].next = nullptr;
		poly_data[num_of_polys].prev = &poly_data[num_of_polys - 1];
		poly_data[num_of_polys - 1].next = &poly_data[num_of_polys];
	}

	num_of_polys++;
}

void render_list_4d_v1::remove_backfaces(const camera_4d_v1 & cam) {
	for (int poly = 0; poly < num_of_polys; poly++) {
		polyf_4d_v1 * curr_poly = poly_point[poly];
		if ((curr_poly == nullptr) ||
			!(curr_poly->state & polyf_4d_v1::POLY_STATE_ACTIVE) ||
			(curr_poly->attribute & polyf_4d_v1::POLY_ATTR_2SIDED) ||
			(curr_poly->state & polyf_4d_v1::POLY_STATE_CLIPPED) ||
			(curr_poly->state & polyf_4d_v1::POLY_STATE_BACKFACE))
			continue;

		vector_4d u, v, n;

		u = curr_poly->t_vertex_list[1] - curr_poly->t_vertex_list[0];
		v = curr_poly->t_vertex_list[2] - curr_poly->t_vertex_list[0];
		n = u.cross(v);

		vector_4d temp = cam.position;
		vector_4d view = temp - curr_poly->t_vertex_list[0];

		float dp = n.dot(view);
		if (dp <= 0.0)
			curr_poly->state |= polyf_4d_v1::POLY_STATE_BACKFACE;
	}
}

void object_4d_v1::transform(matrix_44 matrix_trans, trans_coord coord_select, bool is_trans_basis) {
	switch (coord_select)
	{
	case local_only:
		for (int vertex = 0; vertex < num_of_vertices; vertex++) {
		//	point_4d result = matrix_trans.mul_vector_4d(vertex_list_local[vertex]);
			point_4d result = vertex_list_local[vertex].mul_matrix_44(matrix_trans);
			vertex_list_local[vertex] = result;
		}
		break;
	case trans_only:
		for (int vertex = 0; vertex < num_of_vertices; vertex++) {
		//	point_4d result = matrix_trans.mul_vector_4d(vertex_list_trans[vertex]);
			point_4d result = vertex_list_trans[vertex].mul_matrix_44(matrix_trans);
			vertex_list_trans[vertex] = result;
		}
		break;
	case local_to_trans:
		for (int vertex = 0; vertex < num_of_vertices; vertex++) {
		//	point_4d result = matrix_trans.mul_vector_4d(vertex_list_local[vertex]);
			point_4d result = vertex_list_local[vertex].mul_matrix_44(matrix_trans);
			vertex_list_trans[vertex] = result;
		}
		break;
	default:
		break;
	}

	if (is_trans_basis) {
	//	local_x = matrix_trans.mul_vector_4d(local_x);
	//	local_y = matrix_trans.mul_vector_4d(local_y);
	//	local_z = matrix_trans.mul_vector_4d(local_z);
		local_x = local_x.mul_matrix_44(matrix_trans);
		local_y = local_y.mul_matrix_44(matrix_trans);
		local_z = local_z.mul_matrix_44(matrix_trans);
	}
}

void object_4d_v1::remove_backfaces(const camera_4d_v1 & cam) {
	if (state & OBJECT_STATE_CULLED)
		return;

	for (int poly = 0; poly < num_of_polys; poly++) {
		poly_4d_v1 * curr_poly = &poly_list[poly];

		if (!(curr_poly->state & poly_4d_v1::POLY_STATE_ACTIVE) ||
			(curr_poly->state & poly_4d_v1::POLY_STATE_CLIPPED) ||
			(curr_poly->state & poly_4d_v1::POLY_STATE_BACKFACE) ||
			(curr_poly->attribute & poly_4d_v1::POLY_ATTR_2SIDED))
			continue;

		int v_index_0 = curr_poly->vertices[0];
		int v_index_1 = curr_poly->vertices[1];
		int v_index_2 = curr_poly->vertices[2];

		vector_4d u, v, n;

		u = vertex_list_trans[v_index_1] - vertex_list_trans[v_index_0];
		v = vertex_list_trans[v_index_2] - vertex_list_trans[v_index_0];
		n = u.cross(v);

		vector_4d temp = cam.position;
		vector_4d view = temp - vertex_list_trans[v_index_0];

		float dp = n.dot(view);
		if (dp <= 0.0)
			curr_poly->state |= poly_4d_v1::POLY_STATE_BACKFACE;
	}
}

void object_4d_v1::reset_state() {
	state &= (~OBJECT_STATE_CULLED);

	for (int poly = 0; poly < num_of_polys; poly++) {
		poly_4d_v1 * curr_poly = &poly_list[poly];
		if (!(curr_poly->state & poly_4d_v1::POLY_STATE_ACTIVE))
			continue;
		curr_poly->state &= (~poly_4d_v1::POLY_STATE_CLIPPED);
		curr_poly->state &= (~poly_4d_v1::POLY_STATE_BACKFACE);
	}
}

camera_4d_v1::camera_4d_v1(cam_attri attr,
	point_4d cam_pos,
	vector_4d cam_dir,
	point_4d cam_target,
	float near_clip_z,
	float far_clip_z,
	float fov,
	float viewport_width,
	float viewport_height) {

	this->attribute = attr;
	this->position = cam_pos;
	this->direction = cam_dir;

	u = { 1, 0, 0 };
	v = { 0, 1, 0 };
	n = { 0, 0, 1 };

	this->target = cam_target;

	this->near_clip_z = near_clip_z;
	this->far_clip_z = far_clip_z;

	this->viewport_width = viewport_width;
	this->viewport_height = viewport_height;

	this->viewport_center_x = (viewport_width - 1) / 2;
	this->viewport_center_y = (viewport_height - 1) / 2;

	this->aspect_ratio = viewport_width / viewport_height;

	this->mcam.identity();
	this->mper.identity();
	this->mscr.identity();

	this->fov = fov;

	this->viewplane_width = 2.0;
	this->viewplane_height = 2.0 / this->aspect_ratio;
	
	float tan_fov_div2 = tan(deg_to_rad(fov / 2));
	this->view_dist = 0.5 * this->viewplane_width * tan_fov_div2;	// has problem

	if (fov == 90.0) {
		point_3d pt_origin = { 0.0, 0.0, 0.0 };
//		vector_3d vn = { 1.0, 0.0, -1.0 }; // normal
		this->rt_clip_plane = { pt_origin, { 1.0, 0.0, -1.0 } };
		this->tp_clip_plane = { pt_origin, { 0.0, 1.0, -1.0 } };
		this->lt_clip_plane = { pt_origin, { -1.0, 0.0, -1.0 } };
		this->bt_clip_plane = { pt_origin, { 0.0, -1.0, -1.0 } };
	}
	else {
		point_3d pt_origin = { 0.0, 0.0, 0.0 };
		this->rt_clip_plane = { pt_origin, { view_dist, 0.0f, float(-viewplane_width / 2.0) } };
		this->lt_clip_plane = { pt_origin, { -view_dist, 0.0f, float(-viewplane_width / 2.0) } };
		this->tp_clip_plane = { pt_origin, { view_dist, 0.0f, float(-viewplane_width / 2.0) } };
		this->bt_clip_plane = { pt_origin, { -view_dist, 0.0f, float(-viewplane_width / 2.0) } };
	}
}

void camera_4d_v1::build_matrix_euler(rot_seq seq) {
	matrix_44 mt_inv = { // !!!!warning!!!!
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		-position.x, -position.y, -position.z, 1 },
		mrot;

	float theta_x = direction.x; // angle
	float theta_y = direction.y;
	float theta_z = direction.z;

	theta_x = fmodf(theta_x, 360.0);
	if (theta_x < 0.0) theta_x += 360.0;
	float cos_theta = cos(deg_to_rad(theta_x));	// need to accelerate
	float sin_theta = -sin(deg_to_rad(theta_x));
	matrix_44 mx_inv = {
		1.0, 0.0, 0.0, 0.0,
		0.0, cos_theta, sin_theta, 0.0,
		0.0, -sin_theta, cos_theta, 0.0,
		0.0, 0.0, 0.0, 1.0 };

	theta_y = fmodf(theta_y, 360.0);
	if (theta_y < 0.0) theta_y += 360.0;
	cos_theta = cos(deg_to_rad(theta_y));
	sin_theta = -sin(deg_to_rad(theta_y));
	matrix_44 my_inv = {
		cos_theta, 0.0, -sin_theta, 0.0,
		0.0, 1.0, 0.0, 0.0,
		sin_theta, 0.0, cos_theta, 0.0,
		0.0, 0.0, 0.0, 1.0 };

	theta_z = fmodf(theta_z, 360.0);
	if (theta_z < 0.0) theta_z += 360.0;
	cos_theta = cos(deg_to_rad(theta_z));
	sin_theta = -sin(deg_to_rad(theta_z));
	matrix_44 mz_inv = {
		cos_theta, sin_theta, 0.0, 0.0,
		-sin_theta, cos_theta, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0 };

	switch (seq) {
	case xyz:
		mrot = mx_inv.mul_matrix_44(my_inv).mul_matrix_44(mz_inv);
		break;
	case yxz:
		mrot = my_inv.mul_matrix_44(mx_inv).mul_matrix_44(mz_inv);
		break;
	case xzy:
		mrot = mx_inv.mul_matrix_44(mz_inv).mul_matrix_44(my_inv);
		break;
	case yzx:
		mrot = my_inv.mul_matrix_44(mz_inv).mul_matrix_44(mx_inv);
		break;
	case zyx:
		mrot = mz_inv.mul_matrix_44(my_inv).mul_matrix_44(mx_inv);
		break;
	case zxy:
		mrot = mz_inv.mul_matrix_44(mx_inv).mul_matrix_44(my_inv);
		break;
	default:
		break;
	}

	this->mcam = mt_inv.mul_matrix_44(mrot);
}

void camera_4d_v1::build_matrix_uvn(uvn_mode mode) {
	matrix_44 mt_inv = {
		1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		-position.x, -position.y, -position.z, 1.0 },
		mt_uvn,
		m_tmp;

	if (mode == uvn_spherical) {
		float phi = direction.x;
		float theta = direction.y;

		phi = fmodf(phi, 360.0);
		if (phi < 0.0) phi += 360.0;
		float sin_phi = sin(deg_to_rad(phi));
		float cos_phi = cos(deg_to_rad(phi));

		theta = fmodf(theta, 360.0);
		if (theta < 0.0) theta += 360.0;
		float sin_theta = sin(deg_to_rad(theta));
		float cos_theta = cos(deg_to_rad(theta));

		target.x = -1 * sin_phi * sin_theta; // right to left handed system
		target.y = cos_phi;
		target.z = sin_phi * cos_theta;
	}

	n = target - position;
	v = { 0.0, 1.0, 0.0 };
	u = v.cross(n);
	v = n.cross(u);

	u.normalize();
	v.normalize();
	n.normalize();

	mt_uvn = {
		u.x, v.x, n.x, 0.0,
		u.y, v.y, n.y, 0.0,
		u.z, v.z, n.z, 0.0,
		0.0, 0.0, 0.0, 1.0 };

	mcam = mt_inv.mul_matrix_44(mt_uvn);
}

void model_to_world(object_4d_v1 & obj, trans_coord coord_select){
	if (coord_select == local_to_trans) {
		for (int vertex = 0; vertex < obj.num_of_vertices; vertex++)
			obj.vertex_list_trans[vertex] = obj.vertex_list_local[vertex] + obj.world_position;
	}
	else { // trans_only
		for (int vertex = 0; vertex < obj.num_of_vertices; vertex++)
			obj.vertex_list_trans[vertex] = obj.vertex_list_trans[vertex] + obj.world_position;
	}
}

void model_to_world(render_list_4d_v1 & list, const point_4d & world_pos, trans_coord coord_select) {
	if (coord_select == local_to_trans) {
		for (int poly = 0; poly < list.num_of_polys; poly++) {
			polyf_4d_v1 * curr_poly = list.poly_point[poly];
			// conditions
			for (int vertex = 0; vertex < 3; vertex++)
				curr_poly->t_vertex_list[vertex] = curr_poly->vertex_list[vertex] + world_pos;
		}
	}
	else { // trans_only
		for (int poly = 0; poly < list.num_of_polys; poly++) {
			polyf_4d_v1 * curr_poly = list.poly_point[poly];
			// condition;
			for (int vertex = 0; vertex < 3; vertex++)
				curr_poly->t_vertex_list[vertex] = curr_poly->t_vertex_list[vertex] + world_pos;
		}
	}
}

void world_to_camera(object_4d_v1 & obj, const camera_4d_v1 & camera) {
	for (int vertex = 0; vertex < obj.num_of_vertices; vertex++) {
		obj.vertex_list_trans[vertex] = obj.vertex_list_trans[vertex].mul_matrix_44(camera.mcam);
	}
//  equal to obj.transform(camera.mcam, trans_only);
}

void world_to_camera(render_list_4d_v1 & list, const camera_4d_v1 & camera) {
	for (int poly = 0; poly < list.num_of_polys; poly++) {
		polyf_4d_v1 * curr_poly = list.poly_point[poly];
		// conditions
		for (int vertex = 0; vertex < 3; vertex++)
			curr_poly->t_vertex_list[vertex] = curr_poly->t_vertex_list[vertex].mul_matrix_44(camera.mcam);
	}
}

int camera_4d_v1::cull_object_4d_v1(object_4d_v1 & obj, int cull_flags) {
	point_4d sphere_pos = obj.world_position.mul_matrix_44(mcam);
	
	if (cull_flags & CULL_OBJECT_Z_PLANE) {
		if (((sphere_pos.z - obj.max_radius) > far_clip_z) ||
			((sphere_pos.z + obj.max_radius) < near_clip_z)) {
			obj.state |= object_4d_v1::OBJECT_STATE_CULLED;
			return 1;
		}
	}

	if (cull_flags & CULL_OBJECT_X_PLANE) {
		float z_test = (0.5) * viewplane_width * sphere_pos.z / view_dist;
		if (((sphere_pos.x - obj.max_radius) > z_test) ||
			((sphere_pos.x + obj.max_radius) < z_test)) {
			obj.state |= object_4d_v1::OBJECT_STATE_CULLED;
			return 1;
		}
	}

	if (cull_flags & CULL_OBJECT_Y_PLANE) {
		float z_test = (0.5) * viewplane_height * sphere_pos.z / view_dist;
		if (((sphere_pos.y - obj.max_radius) > z_test) ||
			((sphere_pos.y + obj.max_radius) < z_test)) {
			obj.state |= object_4d_v1::OBJECT_STATE_CULLED;
			return 1;
		}
	}

	// culling fail
	return 0;
}

void camera_to_perspective(object_4d_v1 & obj, const camera_4d_v1 & camera) {
	for (int vertex = 0; vertex < obj.num_of_vertices; vertex++) {
		float z = obj.vertex_list_trans[vertex].z;
		obj.vertex_list_trans[vertex].x = camera.view_dist * obj.vertex_list_trans[vertex].x / z;
		obj.vertex_list_trans[vertex].y = camera.view_dist * obj.vertex_list_trans[vertex].y * camera.aspect_ratio / z;
	}
}

void camera_to_perspective(render_list_4d_v1 & list, const camera_4d_v1 & camera) {
	for (int poly = 0; poly < list.num_of_polys; poly++) {
		polyf_4d_v1 * curr_poly = list.poly_point[poly];

		if (!(curr_poly->state & poly_4d_v1::POLY_STATE_ACTIVE) ||
			(curr_poly->state & poly_4d_v1::POLY_STATE_CLIPPED) ||
			(curr_poly->state & poly_4d_v1::POLY_STATE_BACKFACE) ||
			(curr_poly->attribute & poly_4d_v1::POLY_ATTR_2SIDED))
			continue;

		for (int vertex = 0; vertex < 3; vertex++) {
			float z = curr_poly->t_vertex_list[vertex].z;
			curr_poly->t_vertex_list[vertex].x = camera.view_dist * curr_poly->t_vertex_list[vertex].x / z;
			curr_poly->t_vertex_list[vertex].y = camera.view_dist * curr_poly->t_vertex_list[vertex].y * camera.aspect_ratio / z;
		}
	}
}

void perspective_to_screen(object_4d_v1 & obj, const camera_4d_v1 & camera) {
	float alpha = (0.5 * camera.viewport_width - 0.5);
	float beta = (0.5 * camera.viewport_height - 0.5);
	for (int vertex = 0; vertex < obj.num_of_vertices; vertex++) {
		obj.vertex_list_trans[vertex].x = alpha + alpha * obj.vertex_list_trans[vertex].x;
		obj.vertex_list_trans[vertex].y = beta - beta * obj.vertex_list_trans[vertex].y;
	}
}

void perspective_to_screen(render_list_4d_v1 & list, const camera_4d_v1 & camera) {
	for (int poly = 0; poly < list.num_of_polys; poly++) {
		polyf_4d_v1 * curr_poly = list.poly_point[poly];
		
		if (!(curr_poly->state & poly_4d_v1::POLY_STATE_ACTIVE) ||
			(curr_poly->state & poly_4d_v1::POLY_STATE_CLIPPED) ||
			(curr_poly->state & poly_4d_v1::POLY_STATE_BACKFACE) ||
			(curr_poly->attribute & poly_4d_v1::POLY_ATTR_2SIDED))
			continue;

		float alpha = (0.5 * camera.viewport_width - 0.5);
		float beta = (0.5 * camera.viewport_height - 0.5);

		for (int vertex = 0; vertex < 3; vertex++) {
			curr_poly->t_vertex_list[vertex].x = alpha + alpha * curr_poly->t_vertex_list[vertex].x;
			curr_poly->t_vertex_list[vertex].y = beta - beta * curr_poly->t_vertex_list[vertex].y;
		}
	}
}