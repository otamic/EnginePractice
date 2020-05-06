#include "math_test.h"
#include <cmath>

vector_4d vector_4d::operator+(const vector_4d & vector) {
	vector_4d result;
	result.x = x + vector.x;
	result.y = y + vector.y;
	result.z = z + vector.z;
	result.w = 1;
	return result;
}

vector_4d vector_4d::operator-(const vector_4d & vector) {
	vector_4d result;
	result.x = x - vector.x;
	result.y = y - vector.y;
	result.z = z - vector.z;
	result.w = 1;
	return result;
}

float vector_4d::dot(const vector_4d & vector) {
	return x * vector.x + y * vector.y + z * vector.z;
}

vector_4d vector_4d::cross(const vector_4d & vector) {
	vector_4d result;
	result.x = y * vector.z - z * vector.y;
	result.y = -(x * vector.z - z * vector.x);
	result.z = x * vector.y - y * vector.x;
	result.w = 1;
	return result;
}

vector_4d vector_4d::mul_matrix_44(const matrix_44 & matrix) {
	vector_4d result;
	for (int i = 0; i < 4; i++) {
		result.m[i] = 0.0;
		for (int j = 0; j < 4; j++)
			result.m[i] += matrix.m[j][i] * m[j];
	}
	return result;
}

void vector_4d::normalize() {
	float length = sqrtf(x * x + y * y + z * z);
	if (length < 1e-3) return;
	float length_inv = 1.0 / length;
	x *= length_inv;
	y *= length_inv;
	z *= length_inv;
	w = 1;
}

vector_4d matrix_44::mul_vector_4d(const vector_4d & vector) {
	vector_4d result;
	for (int i = 0; i < 4; i++) {
		result.m[i] = 0.0;
		for (int j = 0; j < 4; j++)
			result.m[i] += m[i][j] * vector.m[j];
	}
	return result;
}

matrix_44 matrix_44::mul_matrix_44(const matrix_44 & matrix) {
	matrix_44 result;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++){
			result.m[i][j] = 0;
			for (int z = 0; z < 4; z++)
				result.m[i][j] += m[i][z] * matrix.m[z][j];
		}
	}
	return result;
}

void matrix_44::identity() {
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			if (i == j)
				m[i][j] = 1;
			else
				m[i][j] = 0;
}

matrix_44 rotate_matrix(float theta_x, float theta_y, float theta_z) {
	static const float EPSILON_E5 = 1e-5;
	float cos_theta, sin_theta;
	
	theta_x = fmodf(theta_x, 360.0);
	theta_x = deg_to_rad(theta_x);

	cos_theta = cos(theta_x);
	sin_theta = sin(theta_x);

	matrix_44 mx = {
		1, 0, 0, 0,
		0, cos_theta, sin_theta, 0,
		0, -sin_theta, cos_theta, 0,
		0, 0, 0, 1 };

	theta_y = fmodf(theta_y, 360.0);
	theta_y = deg_to_rad(theta_y);

	cos_theta = cos(theta_y);
	sin_theta = sin(theta_y);

	matrix_44 my = {
		cos_theta, 0, -sin_theta, 0,
		0, 1, 0, 0,
		sin_theta, 0, cos_theta, 0,
		0, 0, 0, 1 };

	theta_z = fmodf(theta_z, 360.0);
	theta_z = deg_to_rad(theta_z);

	cos_theta = cos(theta_z);
	sin_theta = sin(theta_z);

	matrix_44 mz = {
		cos_theta, sin_theta, 0, 0,
		-sin_theta, cos_theta, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1 };

	return mx.mul_matrix_44(my).mul_matrix_44(mz);

}