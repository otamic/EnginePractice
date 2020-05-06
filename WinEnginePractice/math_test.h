#pragma once

// should use namespace
const float pi = 3.141592654f;

inline float deg_to_rad(float ang) { return ang * pi / 180.0; }

struct matrix_44;

struct vector_3d {
	union {
		float m[3];
		struct {
			float x, y, z;
		};
	};
	vector_3d() { x = y = z = 0.0; }
	vector_3d(float x, float y, float z) :x(x), y(y), z(z) {}
};

typedef vector_3d point_3d;

struct vector_4d {
	union {
		float m[4];
		struct {
			float x, y, z, w;
		};
	};

	vector_4d() { x = y = z = 0.0 ; w = 1.0; }
	vector_4d(float x, float y, float z) { this->x = x; this->y = y; this->z = z; w = 1; }
	vector_4d operator+(const vector_4d & vector);
	vector_4d operator-(const vector_4d & vector);
	float dot(const vector_4d & vector);
	vector_4d cross(const vector_4d & vector);
	vector_4d mul_matrix_44(const matrix_44 & matrix);
	void normalize();
};

typedef vector_4d point_4d;

struct matrix_44 {
	union {
		float m[4][4];
		struct {
			float m00, m01, m02, m03;
			float m10, m11, m12, m13;
			float m20, m21, m22, m23;
			float m30, m31, m32, m33;
		};
	};
	matrix_44() = default;
	matrix_44(
		float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23,
		float m30, float m31, float m32, float m33) {
		this->m00 = m00; this->m01 = m01; this->m02 = m02; this->m03 = m03;
		this->m10 = m10; this->m11 = m11; this->m12 = m12; this->m13 = m13;
		this->m20 = m20; this->m21 = m21; this->m22 = m22; this->m23 = m23;
		this->m30 = m30; this->m31 = m31; this->m32 = m32; this->m33 = m33;
	}
	vector_4d mul_vector_4d(const vector_4d & vector);
	matrix_44 mul_matrix_44(const matrix_44 & matrix);
	void identity();
};

matrix_44 rotate_matrix(float theta_x, float theta_y, float theta_z);





