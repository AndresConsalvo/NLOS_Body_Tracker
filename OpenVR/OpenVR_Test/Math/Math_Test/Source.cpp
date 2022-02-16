#include "Matrix.h"
#include <iostream>

int main() {
	https://ahrs.readthedocs.io/en/latest/filters/angular.html
	ang_rate angle_vector(0, 0, 0);

	omega omega_op(angle_vector);

	double mag = angle_vector.get_magnitude();
	double scale_cos = cos(mag / 2.0);
	double scale_sin = (1 / mag) * sin(mag / 2.0);

	double iMatrixScaled[16] = { 0 };

	double cos_scalar = cos(mag / 2);

	double quat[4] = { 0.0, 0.0, 0.0, 1.0 }; // x, y, z, w
	double new_quat[4] = { 0.0, 0.0, 0.0, 1.0 }; // x, y, z, w

	for (int i = 0; i < 16; i++) {
		iMatrixScaled[i] = omega_op.iMatrix[i] * cos_scalar;
		printf("%f, %f\n", omega_op.iMatrix[i], iMatrixScaled[i]);
	}

	double sin_scalar = sin(mag / 2);
	sin_scalar = sin_scalar * (1 / mag);

	double omegaMatrixScaled[16] = { 0 };

	for (int i = 0; i < 16; i++) {
		omegaMatrixScaled[i] = omega_op.matrix[i] * sin_scalar;
	}

	double matrix_full[16] = { 0 };

	for (int i = 0; i < 16; i++) {
		matrix_full[i] = omegaMatrixScaled[i] + iMatrixScaled[i];
	}

	for (int i = 0; i < 4; i++) {
		new_quat[i] = (matrix_full[i * 4 + 0] * quat[0]) + (matrix_full[i * 4 + 1] * quat[1]) + (matrix_full[i * 4 + 2] * quat[2]) + (matrix_full[i * 4 + 3] * quat[3]);
	}

	printf("x: %f, y: %f, z: %f, w: %f\n", new_quat[0], new_quat[1], new_quat[2], new_quat[3]);
}