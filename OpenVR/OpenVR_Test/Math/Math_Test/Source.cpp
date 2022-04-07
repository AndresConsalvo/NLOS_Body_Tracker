#include "Matrix.h"
#include <iostream>

int main() {
	
	//https://ahrs.readthedocs.io/en/latest/filters/angular.html
	ang_rate angle_vector(3.14, 0, 0);

	omega omega_op(angle_vector);

	double mag = angle_vector.get_magnitude();
	double scale_cos = cos((mag) / 2.0);
	double scale_sin = (1 / mag) * sin((mag) / 2.0);

	double iMatrixScaled[16] = { 0 };

	double old_quat[4] = { 1.0, 0.0, 0.0, 0.0 }; // w, x, y, z
	double new_quat[4] = { 1.0, 0.0, 0.0, 0.0 }; // w, x, y, z

	for (int i = 0; i < 16; i++) {
		iMatrixScaled[i] = omega_op.iMatrix[i] * scale_cos;
	}

	double sin_scalar = sin(mag / 2);
	sin_scalar = sin_scalar * (1 / mag);
	printf("scale_cos: %f\n", scale_cos);
	printf("sin_scalar: %f\n", sin_scalar);

	double omegaMatrixScaled[16] = { 0 };

	for (int i = 0; i < 16; i++) {
		omegaMatrixScaled[i] = omega_op.matrix[i] * sin_scalar;
	}

	double matrix_full[16] = { 0 };

	for (int i = 0; i < 16; i++) {
		matrix_full[i] = omegaMatrixScaled[i] + iMatrixScaled[i];
	}

	for (int i = 0; i < 4; i++) {
		new_quat[i] = (matrix_full[i * 4 + 0] * old_quat[0]) + (matrix_full[i * 4 + 1] * old_quat[1]) + (matrix_full[i * 4 + 2] * old_quat[2]) + (matrix_full[i * 4 + 3] * old_quat[3]);
	}

	double newquatmag = sqrt(pow(new_quat[0], 2) + pow(new_quat[1], 2) + pow(new_quat[2], 2) + pow(new_quat[3], 2));
	new_quat[0] = new_quat[0] / newquatmag;
	new_quat[1] = new_quat[1] / newquatmag;
	new_quat[2] = new_quat[2] / newquatmag;
	new_quat[3] = new_quat[3] / newquatmag;

	printf("w: %f, x: %f, y: %f, z: %f\n", new_quat[0], new_quat[1], new_quat[2], new_quat[3]);
	

	Quaternion old_Quat;
	Vector3_d angular_velocity(0, 1, 0);
	Matrix44_d test_Omega;
	Matrix44_d identity_Matrix;
	test_Omega.set_as_Omega_Matrix(angular_velocity);
	identity_Matrix.set_as_Identity();
	double ang_mag = angular_velocity.getMag();

	double scale_Ident = cos(ang_mag / 2.0);
	double scale_Omega = (1.0 / ang_mag) * sin(ang_mag / 2);

	printf("ang_mag: %f\n", ang_mag);
	printf("scale_Ident: %f\n", scale_Ident);
	printf("scale_Omega: %f\n", scale_Omega);

	identity_Matrix.scale_Matrix(scale_Ident);
	test_Omega.scale_Matrix(scale_Omega);
	Matrix44_d result = test_Omega + identity_Matrix;

	Quaternion new_Quat = result.getNewQuat(old_Quat);
	new_Quat.normalize();

	printf("w: %f, x: %f, y: %f, z: %f\n", new_Quat.w, new_Quat.x, new_Quat.y, new_Quat.z);
	


	/*
	
	// https://www.andre-gaschler.com/rotationconverter/
	Vector3_d x_axis(1.0, 1.0, 2.0);
	Quaternion rotation(0.707, 0, 0.707, 0);
	Quaternion from_x_axis(0, x_axis.x, x_axis.y, x_axis.z);
	Quaternion newRotation = rotation * from_x_axis * rotation.GetInverse();
	Vector3_d y_axis(newRotation.x, newRotation.y, newRotation.z);
	printf("Quat, w: %f, x: %f, y: %f, z: %f\n", rotation.w, rotation.x, rotation.y, rotation.z);
	printf("x: %f, y: %f, z: %f\n", y_axis.x, y_axis.y, y_axis.z);
	*/
	return 0;
}