#include "math_helpers.h"

// 4x4 Matrix:
// 0 1 2 3
// 4 5 6 7
// 8 9 10 11
// 12 13 14 15

// from https://ahrs.readthedocs.io/en/latest/filters/angular.html
// 
// for error logging


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
// https://ahrs.readthedocs.io/en/latest/filters/angular.html
// change to using pointers
void getNewPose(int limb, ang_rate angle_vector, double elapsed_time_s) {
	char log_str[100];
	DriverPose_t* last_pose;
	last_pose = &waist_pose;
	switch (limb) {
		case WAIST:
			last_pose = &waist_pose;
			break;
		case LFOOT:
			last_pose = &lfoot_pose;
			break;
		case RFOOT:
			last_pose = &rfoot_pose;
			break;
	}

	HmdQuaternion_t quat;

	quat.x, quat.y, quat.z = 0;
	quat.w = 1;

	last_pose->qWorldFromDriverRotation = quat;
	last_pose->qDriverFromHeadRotation = quat;

	omega omega_op(angle_vector);

	double mag = angle_vector.get_magnitude();
	double scale_cos = cos((mag * elapsed_time_s) / 2.0);
	double scale_sin = (1 / mag) * sin((mag * elapsed_time_s) / 2.0);

	double iMatrixScaled[16] = { 0 };

	double old_quat[4] = { last_pose->qRotation.w, last_pose->qRotation.x, last_pose->qRotation.y, last_pose->qRotation.z }; // w, x, y, z
	double new_quat[4] = { 1.0, 0.0, 0.0, 0.0 }; // w, x, y, z

	snprintf(log_str, 100, "Elapsed time in : %f (s)", elapsed_time_s);
	VRDriverLog()->Log(log_str);

	snprintf(log_str, 100, "Last pose: %f, %f, %f, %f\n", last_pose->qRotation.w, last_pose->qRotation.x, last_pose->qRotation.y, last_pose->qRotation.z);
	VRDriverLog()->Log(log_str);

	for (int i = 0; i < 16; i++) {
		iMatrixScaled[i] = omega_op.iMatrix[i] * scale_cos;
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

	double newquatmag = sqrt(pow(new_quat[0], 2) + pow(new_quat[1], 2) + pow(new_quat[2], 2) + pow(new_quat[3], 2));
	last_pose->qRotation.w = new_quat[0] / newquatmag;
	last_pose->qRotation.x = new_quat[1] / newquatmag;
	last_pose->qRotation.y = new_quat[2] / newquatmag;
	last_pose->qRotation.z = new_quat[3] / newquatmag;
	
	snprintf(log_str, 100, "%f, %f, %f, %f\n", last_pose->qRotation.w, last_pose->qRotation.x, last_pose->qRotation.y, last_pose->qRotation.z);
	VRDriverLog()->Log(log_str);

	snprintf(log_str, 100, "Driver pos: x: %f, y: %f, z: %f\n", hmd_pose.vecPosition[0], hmd_pose.vecPosition[1], hmd_pose.vecPosition[2]);

	// from https://automaticaddison.com/how-to-convert-a-quaternion-into-euler-angles-in-python/

	double w = last_pose->qRotation.w;
	double x = last_pose->qRotation.x;
	double y = last_pose->qRotation.y;
	double z = last_pose->qRotation.z;
	double roll, pitch, yaw;
	double t0, t1, t2, t3, t4;

	t0 = 2.0 * (w * x + y * z);
	t1 = 1.0 - (2.0 * (x * x + y * y));
	t2 = 2.0 * (w * y - z * x);
	if (t2 > 1.0) {
		t2 = 1.0;
	} else if (t2 < -1.0) {
		t2 = -1.0;
	}

	t3 = 2.0 * (w * z + x * y);
	t4 = 1.0 - 2.0 * (y * y + z * z);

	roll = atan2(t0, t1);
	pitch = -asin(t2);
	yaw = atan2(t3, t4);

	snprintf(log_str, 100, "Pos of HMD: pos_x: %f, pos_y: %f, pos_z: %f\n", hmd_pose.vecPosition[0], hmd_pose.vecPosition[1], hmd_pose.vecPosition[2]);
	VRDriverLog()->Log(log_str);

	switch (limb) {
	case WAIST:
		snprintf(log_str, 100, "Roll: %f, Pitch: %f, Yaw %f\n", roll, pitch, yaw);
		VRDriverLog()->Log(log_str);
		last_pose->vecPosition[0] = hmd_pose.vecPosition[0] - Head_to_Waist_len_m * sin(pitch);
		last_pose->vecPosition[1] = hmd_pose.vecPosition[1] - Head_to_Waist_len_m * cos(pitch);
		last_pose->vecPosition[2] = hmd_pose.vecPosition[2] - Head_to_Waist_len_m * sin(roll);

		break;
	case LFOOT:
		last_pose->vecPosition[0] = waist_pose.vecPosition[0];
		last_pose->vecPosition[1] = waist_pose.vecPosition[1] - Waist_to_Foot_len_m;
		if (last_pose->vecPosition[1] < 0) {
			last_pose->vecPosition[1] = 0;
		}
		last_pose->vecPosition[2] = waist_pose.vecPosition[2] + 0.1;
		break;
	case RFOOT:
		last_pose->vecPosition[0] = waist_pose.vecPosition[0];
		last_pose->vecPosition[1] = waist_pose.vecPosition[1] - Waist_to_Foot_len_m;
		if (last_pose->vecPosition[1] < 0) {
			last_pose->vecPosition[1] = 0;
		}
		last_pose->vecPosition[2] = waist_pose.vecPosition[2] - 0.1;
		break;
	}



	return;
}