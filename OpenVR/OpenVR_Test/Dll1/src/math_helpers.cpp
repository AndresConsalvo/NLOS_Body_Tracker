#include "math_helpers.h"

// 4x4 Matrix:
// 0 1 2 3
// 4 5 6 7
// 8 9 10 11
// 12 13 14 15


// https://ahrs.readthedocs.io/en/latest/filters/angular.html
void getNewPose(int limb, Vector3_d angle_vector, double elapsed_time_s) {
	char log_str[100];
	DriverPose_t* pose;
	pose = &waist_pose;
	switch (limb) {
		case WAIST:
			pose = &waist_pose;
			break;
		case LFOOT:
			pose = &lfoot_pose;
			break;
		case RFOOT:
			pose = &rfoot_pose;
			break;
	}

	HmdQuaternion_t quat;

	quat.x, quat.y, quat.z = 0;
	quat.w = 1;

	pose->qWorldFromDriverRotation = quat;
	pose->qDriverFromHeadRotation = quat;


	Quaternion q_t0(pose->qRotation.w, pose->qRotation.x, pose->qRotation.y, pose->qRotation.z);
	Matrix44_d omega_Matrix;
	Matrix44_d identity_Matrix;
	omega_Matrix.set_as_Omega_Matrix(angle_vector);
	identity_Matrix.set_as_Identity();
	double ang_mag = angle_vector.getMag();


	double scale_Ident = cos(ang_mag / 2.0);
	double scale_Omega = (1.0 / ang_mag) * sin(ang_mag / 2);
	identity_Matrix.scale_Matrix(scale_Ident);
	omega_Matrix.scale_Matrix(scale_Omega);
	Matrix44_d result = omega_Matrix + identity_Matrix;

	Quaternion q_t1 = result.getNewQuat(q_t0);
	q_t1.normalize();

	pose->qRotation.w = q_t1.w;
	pose->qRotation.x = q_t1.x;
	pose->qRotation.y = q_t1.y;
	pose->qRotation.z = q_t1.z;
	
	snprintf(log_str, 100, "%f, %f, %f, %f\n", pose->qRotation.w, pose->qRotation.x, pose->qRotation.y, pose->qRotation.z);
	VRDriverLog()->Log(log_str);

	snprintf(log_str, 100, "Driver pos: x: %f, y: %f, z: %f\n", hmd_pose.vecPosition[0], hmd_pose.vecPosition[1], hmd_pose.vecPosition[2]);

	// from https://automaticaddison.com/how-to-convert-a-quaternion-into-euler-angles-in-python/
	Quaternion posVec, newPos;

	switch (limb) {
	case WAIST:

		posVec = Quaternion(0, 0, -0.8, 0);
		newPos = q_t1 * posVec * q_t1.GetInverse();

		pose->vecPosition[0] = hmd_pose.vecPosition[0] + newPos.x;
		pose->vecPosition[1] = hmd_pose.vecPosition[1] + newPos.y;
		pose->vecPosition[2] = hmd_pose.vecPosition[2] + newPos.z;

		break;
	case LFOOT:
		pose->vecPosition[0] = 0;
		pose->vecPosition[1] = 0.8;
		if (pose->vecPosition[1] < 0) {
			pose->vecPosition[1] = 0;
		}
		pose->vecPosition[2] = -0.1;
		break;
	case RFOOT:
		pose->vecPosition[0] = 0;
		pose->vecPosition[1] = waist_pose.vecPosition[1] - Waist_to_Foot_len_m;
		if (pose->vecPosition[1] < 0) {
			pose->vecPosition[1] = 0;
		}
		pose->vecPosition[2] = 0.1;
		break;
	}



	return;
}