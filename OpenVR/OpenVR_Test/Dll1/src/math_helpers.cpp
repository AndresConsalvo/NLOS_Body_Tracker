#include "math_helpers.h"

// 4x4 Matrix:
// 0 1 2 3
// 4 5 6 7
// 8 9 10 11
// 12 13 14 15


//// https://ahrs.readthedocs.io/en/latest/filters/angular.html
//void getNewPose(int limb, Vector3_d angle_vector, double elapsed_time_s) {
//	char log_str[100];
//	DriverPose_t* pose;
//	pose = &waist_pose;
//	switch (limb) {
//		case WAIST:
//			pose = &waist_pose;
//			break;
//		case LFOOT:
//			pose = &lfoot_pose;
//			break;
//		case RFOOT:
//			pose = &rfoot_pose;
//			break;
//	}
//
//	// Move to initialization
//	HmdQuaternion_t quat;
//
//	quat.x, quat.y, quat.z = 0;
//	quat.w = 1;
//
//	pose->qWorldFromDriverRotation = quat;
//	pose->qDriverFromHeadRotation = quat;
//
//	Quaternion q_t0(pose->qRotation.w, pose->qRotation.x, pose->qRotation.y, pose->qRotation.z);
//	snprintf(log_str, 100, "q_t0: %f, %f, %f, %f\n", q_t0.w, q_t0.x, q_t0.y, q_t0.z);
//	VRDriverLog()->Log(log_str);
//	Matrix44_d omega_Matrix;
//	Matrix44_d identity_Matrix;
//	omega_Matrix.set_as_Omega_Matrix(angle_vector);
//	identity_Matrix.set_as_Identity();
//	double ang_mag = angle_vector.getMag();
//
//	// Can't be dividing by zero now, can we?
//	if (checkIfZero(ang_mag, 0.00001)) {
//		ang_mag = 0.00001;
//	}
//	
//	snprintf(log_str, 100, "Scale ident: %f\n", ang_mag);
//	VRDriverLog()->Log(log_str);
//
//	double scale_Ident = cos(ang_mag * elapsed_time_s / 2.0);
//	snprintf(log_str, 100, "Scale ident: %f\n", scale_Ident);
//	VRDriverLog()->Log(log_str);
//	double scale_Omega = (1.0 / ang_mag) * sin(ang_mag * elapsed_time_s / 2);
//
//	snprintf(log_str, 100, "Scale omega: %f\n", scale_Omega);
//	VRDriverLog()->Log(log_str);
//
//	identity_Matrix.scale_Matrix(scale_Ident);
//	omega_Matrix.scale_Matrix(scale_Omega);
//	Matrix44_d result = omega_Matrix + identity_Matrix;
//
//	Quaternion q_t1 = result.getNewQuat(&q_t0);
//
//
//
//	q_t1.normalize();
//
//	pose->qRotation.w = q_t1.w;
//	pose->qRotation.x = q_t1.x;
//	pose->qRotation.y = q_t1.y;
//	pose->qRotation.z = q_t1.z;
//
//
//	Quaternion posVec, newPos;
//
//	switch (limb) {
//	case WAIST:
//		VRDriverLog()->Log("Waist pos updated!");
//		posVec = Quaternion(0, 0, -Neck_to_Waist, 0);
//		newPos = q_t1 * posVec * q_t1.GetInverse();
//
//		pose->vecPosition[0] = neck_pose.vecPosition[0] + newPos.x;
//		pose->vecPosition[1] = neck_pose.vecPosition[1] + newPos.y;
//
//		if (pose->vecPosition[1] < 0) {
//			pose->vecPosition[1] = 0;
//		}
//
//		pose->vecPosition[2] = neck_pose.vecPosition[2] + newPos.z;
//
//		break;
//	case LFOOT:
//		posVec = Quaternion(0, 0, -Hip_to_Foot_len_m, 0);
//		newPos = q_t1 * posVec * q_t1.GetInverse();
//
//		pose->vecPosition[0] = lhip_pose.vecPosition[0] + newPos.x;
//		pose->vecPosition[1] = lhip_pose.vecPosition[1] + newPos.y;
//		if (pose->vecPosition[1] < ankle_to_ground) {
//			pose->vecPosition[1] = ankle_to_ground;
//		}
//		pose->vecPosition[2] = lhip_pose.vecPosition[2] + newPos.z;
//		break;
//	case RFOOT:
//		posVec = Quaternion(0, 0, -Hip_to_Foot_len_m, 0);
//		newPos = q_t1 * posVec * q_t1.GetInverse();
//
//		pose->vecPosition[0] = rhip_pose.vecPosition[0] + newPos.x;
//		pose->vecPosition[1] = rhip_pose.vecPosition[1] + newPos.y;
//		if (pose->vecPosition[1] < ankle_to_ground) {
//			pose->vecPosition[1] = ankle_to_ground;
//		}
//		pose->vecPosition[2] = rhip_pose.vecPosition[2] + newPos.z;
//		break;
//	}
//
//	return;
//}
//
//// Rattle me bones
//// Updates body parts that aren't explicitly tracked.
//// Explanation: Some parts of the body simply don't rotate, but serve as good guidelines to calculate the position of the rest of the body.
//// i.e. when the head rotates, the neck doesn't rotate but is instead in a fixed position. Since the offset from the headset to the head will always be fixed,
//// we can find the position of the neck however the head is rotated.
//void updateSkeleton() {
//
//	Quaternion newPos;
//
//	// Head position
//	TrackedDevicePose_t device_pose[10];
//	VRServerDriverHost()->GetRawTrackedDevicePoses(0, device_pose, 10);
//
//	HmdMatrix34_t space_matrix = device_pose[k_unTrackedDeviceIndex_Hmd].mDeviceToAbsoluteTracking;
//
//	hmd_pose.vecPosition[0] = space_matrix.m[0][3];
//	hmd_pose.vecPosition[1] = space_matrix.m[1][3];
//	hmd_pose.vecPosition[2] = space_matrix.m[2][3];
//
//	Quaternion hmd_quat = getQuaternionFromHMD(space_matrix);
//
//	// I don't think it's actually necessary to keep the rotation of the hmd, just where it's at.
//	//hmd_pose.qRotation.w = hmd_quat.w;
//	//hmd_pose.qRotation.x = hmd_quat.x;
//	//hmd_pose.qRotation.y = hmd_quat.y;
//	//hmd_pose.qRotation.z = hmd_quat.z;
//
//	
//	// Set neck position relative to HMD
//	Quaternion neckPos(0, 0, -Head_to_Neck, HMD_to_Head);
//	newPos = hmd_quat * neckPos * hmd_quat.GetInverse();
//
//	neck_pose.vecPosition[0] = hmd_pose.vecPosition[0] + newPos.x;
//	neck_pose.vecPosition[1] = hmd_pose.vecPosition[1] + newPos.y;
//	neck_pose.vecPosition[2] = hmd_pose.vecPosition[2] + newPos.z;
//
//	// Set hip position relative to waist
//	Quaternion tailbonePos(0, 0, -Waist_to_Hip, 0);
//	Quaternion waistRotation(waist_pose.qRotation.w, waist_pose.qRotation.x, waist_pose.qRotation.y, waist_pose.qRotation.z);
//	newPos = waistRotation * tailbonePos * waistRotation.GetInverse();
//
//	tailbone_pose.vecPosition[0] = waist_pose.vecPosition[0] + newPos.x;
//	tailbone_pose.vecPosition[1] = waist_pose.vecPosition[1] + newPos.y;
//	tailbone_pose.vecPosition[2] = waist_pose.vecPosition[2] + newPos.z;
//
//	Quaternion lhipPos(0, -hip_width / 2.0, 0, 0);
//	newPos = waistRotation * lhipPos * waistRotation.GetInverse();
//	lhip_pose.vecPosition[0] = tailbone_pose.vecPosition[0] + newPos.x;
//	lhip_pose.vecPosition[1] = tailbone_pose.vecPosition[1] + newPos.y;
//	lhip_pose.vecPosition[2] = tailbone_pose.vecPosition[2] + newPos.z;
//
//	Quaternion rhipPos(0, hip_width / 2.0, 0, 0);
//	newPos = waistRotation * rhipPos * waistRotation.GetInverse();
//	rhip_pose.vecPosition[0] = tailbone_pose.vecPosition[0] + newPos.x;
//	rhip_pose.vecPosition[1] = tailbone_pose.vecPosition[1] + newPos.y;
//	rhip_pose.vecPosition[2] = tailbone_pose.vecPosition[2] + newPos.z;
//
//}
//
//bool checkIfZero(double& val, double threshold) {
//	return (abs(val) <= threshold);
//}

// https://github.com/osudrl/OpenVR-Tracking-Example/blob/e17119b5129a3d77e4054f0fa30401a13daaf1dd/HTC%20Lighthouse%20Tracking%20Example/LighthouseTracking.cpp#L166-L177
Quaternion getQuaternionFromHMD(HmdMatrix34_t& matrix) {
	TrackedDevicePose_t device_pose[10];
	VRServerDriverHost()->GetRawTrackedDevicePoses(0, device_pose, 10);

	HmdMatrix34_t space_matrix = device_pose[k_unTrackedDeviceIndex_Hmd].mDeviceToAbsoluteTracking;

	Quaternion newQuat;

	newQuat.w = sqrt(fmax(0, 1 + matrix.m[0][0] + matrix.m[1][1] + matrix.m[2][2])) / 2;
	newQuat.x = sqrt(fmax(0, 1 + matrix.m[0][0] - matrix.m[1][1] - matrix.m[2][2])) / 2;
	newQuat.y = sqrt(fmax(0, 1 - matrix.m[0][0] + matrix.m[1][1] - matrix.m[2][2])) / 2;
	newQuat.z = sqrt(fmax(0, 1 - matrix.m[0][0] - matrix.m[1][1] + matrix.m[2][2])) / 2;
	newQuat.x = copysign(newQuat.x, matrix.m[2][1] - matrix.m[1][2]);
	newQuat.y = copysign(newQuat.y, matrix.m[0][2] - matrix.m[2][0]);
	newQuat.z = copysign(newQuat.z, matrix.m[1][0] - matrix.m[0][1]);

	return newQuat;
}