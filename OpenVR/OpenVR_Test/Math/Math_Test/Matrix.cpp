#include "Matrix.h"
#include <math.h>

// Closed form solution are the best!
// 
// 4x4 Matrix:
// 0 1 2 3
// 4 5 6 7
// 8 9 10 11
// 12 13 14 15

// from https://ahrs.readthedocs.io/en/latest/filters/angular.html

omega::omega(ang_rate measured_angle) {
	matrix[0] = 0;
	matrix[1] = -measured_angle.x;
	matrix[2] = -measured_angle.y;
	matrix[3] = -measured_angle.z;

	matrix[4] = measured_angle.x;
	matrix[5] = 0;
	matrix[6] = measured_angle.z;
	matrix[7] = -measured_angle.y;

	matrix[8] = measured_angle.y;
	matrix[9] = -measured_angle.z;
	matrix[10] = 0;
	matrix[11] = measured_angle.x;

	matrix[12] = measured_angle.z;
	matrix[13] = measured_angle.y;
	matrix[14] = -measured_angle.x;
	matrix[15] = 0;
}

double* omega::getMatrix() {
	return matrix;
}

ang_rate::ang_rate(double ang_x, double ang_y, double ang_z) {
	this->x = ang_x;
	this->y = ang_y;
	this->z = ang_z;
}

double ang_rate::get_magnitude() {
	return sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
}



// Vectors
Vector3_d::Vector3_d() {
	this->x = 0;
	this->y = 0;
	this->z = 0;
}

Vector3_d::Vector3_d(double x, double y, double z) {
	this->x = x;
	this->y = y;
	this->z = z;
}

double Vector3_d::getMag() {
	return sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
}

// Quaternions
Quaternion Quaternion::operator*(const Quaternion& r) {
	Quaternion q;
	Quaternion l = *this;

	q.w = l.w * r.w - l.x * r.x - l.y * r.y - l.z * r.z;
	q.x = l.w * r.x + l.x * r.w + l.y * r.z - l.z * r.y;
	q.y = l.w * r.y - l.x * r.z + l.y * r.w + l.z * r.x;
	q.z = l.w * r.z + l.x * r.y - l.y * r.x + l.z * r.w;

	return q;
}

Quaternion::Quaternion() {
	this->w = 1.0;
	this->x = 0.0;
	this->y = 0.0;
	this->z = 0.0;
}

Quaternion::Quaternion(double w, double x, double y, double z) {
	this->w = w;
	this->x = x;
	this->y = y;
	this->z = z;
}

Quaternion Quaternion::GetInverse() {
	return Quaternion(w, -x, -y, -z);
}

void Quaternion::normalize() {
	double mag = sqrt(pow(w, 2) + pow(x, 2) + pow(y, 2) + pow(z, 2));
	w /= mag;
	x /= mag;
	y /= mag;
	z /= mag;

	return;
}

// Matrices

Matrix44_d::Matrix44_d() {
	m[0] = 0;
	m[1] = 0;
	m[2] = 0;
	m[3] = 0;

	m[4] = 0;
	m[5] = 0;
	m[6] = 0;
	m[7] = 0;

	m[8] = 0;
	m[9] = 0;
	m[10] = 0;
	m[11] = 0;

	m[12] = 0;
	m[13] = 0;
	m[14] = 0;
	m[15] = 0;
}

void Matrix44_d::set_as_Identity() {
	m[0] = 1;
	m[1] = 0;
	m[2] = 0;
	m[3] = 0;

	m[4] = 0;
	m[5] = 1;
	m[6] = 0;
	m[7] = 0;

	m[8] = 0;
	m[9] = 0;
	m[10] = 1;
	m[11] = 0;

	m[12] = 0;
	m[13] = 0;
	m[14] = 0;
	m[15] = 1;

	return;
};

void Matrix44_d::set_as_Omega_Matrix(Vector3_d ang_vel) {
	m[0] = 0;
	m[1] = -ang_vel.x;
	m[2] = -ang_vel.y;
	m[3] = -ang_vel.z;

	m[4] = ang_vel.x;
	m[5] = 0;
	m[6] = ang_vel.z;
	m[7] = -ang_vel.y;

	m[8] = ang_vel.y;
	m[9] = -ang_vel.z;
	m[10] = 0;
	m[11] = ang_vel.x;

	m[12] = ang_vel.z;
	m[13] = ang_vel.y;
	m[14] = -ang_vel.x;
	m[15] = 0;

	
	return;
}

void Matrix44_d::scale_Matrix(double scale) {
	for (int i = 0; i < 16; i++) {
		m[i] *= scale;
	}
}

Quaternion Matrix44_d::getNewQuat(Quaternion& q_t0) {
	Quaternion q_t1;

	q_t1.w = m[0] * q_t0.w + m[1] * q_t0.x + m[2] * q_t0.y + m[3] * q_t0.z;
	q_t1.x = m[4] * q_t0.w + m[5] * q_t0.x + m[6] * q_t0.y + m[7] * q_t0.z;
	q_t1.y = m[8] * q_t0.w + m[9] * q_t0.x + m[10] * q_t0.y + m[11] * q_t0.z;
	q_t1.z = m[12] * q_t0.w + m[13] * q_t0.x + m[14] * q_t0.y + m[15] * q_t0.z;

	return q_t1;
}

Matrix44_d Matrix44_d::operator+(const Matrix44_d& r) {
	Matrix44_d result;
	Matrix44_d l = *this;

	for (int i = 0; i < 16; i++) {
		result.m[i] = l.m[i] + r.m[i];
	}

	return result;
}
