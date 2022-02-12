#include "Matrix.h"

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