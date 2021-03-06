#pragma once

#define _USE_MATH_DEFINES


#include <cmath>
#pragma comment(lib, "ws2_32.lib")

// Bare minimum for quaternions, and for some vector math.
// Why implement everything when you can implement the bare minimum to get it working?
// 
// 4x4 Matrix:
// 0 1 2 3
// 4 5 6 7
// 8 9 10 11
// 12 13 14 15

struct Vector3_d {
	float x, y, z = 0;

	Vector3_d();

	Vector3_d(float x, float y, float z);

	float getMag();
};

struct Quaternion {
	float w = 1.0;
	float x, y, z = 0.0;

	Quaternion();

	Quaternion(float w, float x, float y, float z);

	Quaternion operator*(const Quaternion&);

	Quaternion GetInverse();

	void normalize();
};

struct Matrix44_d {
	float m[16];

	Matrix44_d();

	void set_as_Omega_Matrix(Vector3_d ang_vel);

	void set_as_Identity();

	void scale_Matrix(float scale);

	Quaternion getNewQuat(Quaternion* q_t0);

	// Operators
	Matrix44_d operator+(const Matrix44_d& r);
};