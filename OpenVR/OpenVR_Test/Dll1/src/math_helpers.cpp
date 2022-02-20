#include "math_helpers.h"

// 4x4 Matrix:
// 0 1 2 3
// 4 5 6 7
// 8 9 10 11
// 12 13 14 15

// from https://ahrs.readthedocs.io/en/latest/filters/angular.html


ang_rate::ang_rate(double ang_x, double ang_y, double ang_z) {
	this->x = ang_x;
	this->y = ang_y;
	this->z = ang_z;
}

double ang_rate::get_magnitude() {
	return sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
}

omega::omega(ang_rate angular_rate) {
	matrix[0] = 0;
	matrix[1] = -angular_rate.x;
	matrix[2] = -angular_rate.y;
	matrix[3] = -angular_rate.z;

	matrix[4] = angular_rate.x;
	matrix[5] = 0;
	matrix[6] = angular_rate.z;
	matrix[7] = -angular_rate.y;

	matrix[8] = angular_rate.y;
	matrix[9] = -angular_rate.z;
	matrix[10] = 0;
	matrix[11] = angular_rate.x;

	matrix[12] = angular_rate.z;
	matrix[13] = angular_rate.y;
	matrix[14] = -angular_rate.x;
	matrix[15] = 0;
}

double* omega::getMatrix() {
	return matrix;
}

DriverPose_t getNewPose(DriverPose_t last_pose, ang_rate angle_vector, double elapsed_time_s) {
	char log_str[100];
	DriverPose_t pose = { 0 };

	HmdQuaternion_t quat;

	quat.x, quat.y, quat.z = 0;
	quat.w = 1;

	pose.qWorldFromDriverRotation = quat;
	pose.qDriverFromHeadRotation = quat;

	// https://ahrs.readthedocs.io/en/latest/filters/angular.html
	omega omega_op(angle_vector);

	double mag = angle_vector.get_magnitude();
	double scale_cos = cos((mag * elapsed_time_s) / 2.0);
	double scale_sin = (1 / mag) * sin((mag * elapsed_time_s) / 2.0);

	double iMatrixScaled[16] = { 0 };

	double old_quat[4] = { last_pose.qRotation.w, last_pose.qRotation.x, last_pose.qRotation.y, last_pose.qRotation.z }; // w, x, y, z
	double new_quat[4] = { 1.0, 0.0, 0.0, 0.0 }; // w, x, y, z
	snprintf(log_str, 100, "Elapsed time in : %f (ms)", elapsed_time_s);
	VRDriverLog()->Log(log_str);

	snprintf(log_str, 100, "Last pose: %f, %f, %f, %f\n", last_pose.qRotation.w, last_pose.qRotation.x, last_pose.qRotation.y, last_pose.qRotation.z);
	VRDriverLog()->Log(log_str);

	for (int i = 0; i < 16; i++) {
		iMatrixScaled[i] = omega_op.iMatrix[i] * scale_cos;
		//printf("%f, %f\n", omega_op.iMatrix[i], iMatrixScaled[i]);
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
		new_quat[i] = (matrix_full[i * 4 + 0] * old_quat[0]) + (matrix_full[i * 4 + 1] * old_quat[1]) + (matrix_full[i * 4 + 2] * old_quat[2]) + (matrix_full[i * 4 + 3] * old_quat[3]);
	}

	//printf("x: %f, y: %f, z: %f, w: %f\n", new_quat[0], new_quat[1], new_quat[2], new_quat[3]);
	double newquatmag = sqrt(pow(new_quat[0], 2) + pow(new_quat[1], 2) + pow(new_quat[2], 2) + pow(new_quat[3], 2));
	pose.qRotation.w = new_quat[0] / newquatmag;
	pose.qRotation.x = new_quat[1] / newquatmag;
	pose.qRotation.y = new_quat[2] / newquatmag;
	pose.qRotation.z = new_quat[3] / newquatmag;
	
	snprintf(log_str, 100, "%f, %f, %f, %f\n", pose.qRotation.w, pose.qRotation.x, pose.qRotation.y, pose.qRotation.z);
	VRDriverLog()->Log(log_str);

	pose.vecPosition[0] = 1.0;
	pose.vecPosition[1] = 1.0;
	pose.vecPosition[2] = 0;

	pose.poseIsValid = true;
	pose.result = TrackingResult_Running_OK;
	pose.deviceIsConnected = true;

	return pose;
}