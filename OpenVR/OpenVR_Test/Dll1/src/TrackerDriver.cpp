#include <TrackerDriver.h>

#define PORT 20000

// for testing UDP stuff
const char* srcIP = "192.168.1.59";
SOCKET sock;
sockaddr_in local;
WSADATA wsaData;

u_long iMode = 1;

short Gx, Gy, Gz, Ax, Ay, Az = 0;
double ang_x, ang_y, ang_z, acc_x, acc_y, acc_z = 0;

double cy, sy, cp, sp, cr, sr = 0;

DriverPose_t last_pose = { 0 };

EVRInitError TrackerDriver::Activate(uint32_t unObjectId) {
	objID = unObjectId;

	// Props. Like JavaScript?
	ulPropertyContainer = VRProperties()->TrackedDeviceToPropertyContainer(unObjectId);

	// Tracked device is opting out of left/right hand selection -- According to API documentation
	VRProperties()->SetInt32Property(ulPropertyContainer, Prop_ControllerRoleHint_Int32, ETrackedControllerRole::TrackedControllerRole_OptOut);
	VRProperties()->SetInt32Property(ulPropertyContainer, Prop_DeviceClass_Int32, TrackedDeviceClass_GenericTracker);

	// This describes the path to get to the inputprofile which is ABSOLUTELY NECESSARY.
	// Testing from earlier showed that even if it shouldn't be running the driver is still detected. Maybe when it runs it, reinstall the profile each time?
	VRProperties()->SetStringProperty(ulPropertyContainer, Prop_InputProfilePath_String, "{NLOS}/input/tracker_profile.json");

	// Placeholder frame; credit to https://github.com/m9cd0n9ld/IMU-VR-Full-Body-Tracker
	VRProperties()->SetStringProperty(ulPropertyContainer, Prop_RenderModelName_String, "{NLOS}/rendermodels/frame");

	VRProperties()->SetBoolProperty(ulPropertyContainer, Prop_HasDisplayComponent_Bool, false);
	VRProperties()->SetBoolProperty(ulPropertyContainer, Prop_HasCameraComponent_Bool, false);
	VRProperties()->SetBoolProperty(ulPropertyContainer, Prop_HasDriverDirectModeComponent_Bool, false);
	VRProperties()->SetBoolProperty(ulPropertyContainer, Prop_HasVirtualDisplayComponent_Bool, false);

	// Ignored as hand assignment
	VRProperties()->SetInt32Property(ulPropertyContainer, Prop_ControllerHandSelectionPriority_Int32, -1);

	VRDriverLog()->Log("Initializing UDP");
	UDP_init();

	last_pose.qRotation.w = 1.0;
	last_pose.qRotation.x = 0.0;
	last_pose.qRotation.y = 0.0;
	last_pose.qRotation.z = 0.0;

	last_pose.vecPosition[0] = 0;
	last_pose.vecPosition[1] = 1.0;
	last_pose.vecPosition[2] = 0;


	return VRInitError_None;
}

void TrackerDriver::Deactivate() {
	WSACleanup();
	VRDriverLog()->Log("Shutting down tracker");
	objID = vr::k_unTrackedDeviceIndexInvalid;
}

void TrackerDriver::EnterStandby() {
	// Don't mess with this.
}

void* TrackerDriver::GetComponent(const char* pchComponentNameandVersion) {
	if (strcmp(IVRDisplayComponent_Version, pchComponentNameandVersion) == 0) {
		return (IVRDisplayComponent*) this;
	}

	return NULL;
}

void TrackerDriver::DebugRequest(const char* pchRequest, char* pchResponseBuffer, uint32_t unResponseBufferSize) {
	if (unResponseBufferSize >= 1) {
		pchResponseBuffer[0] = 0;
	}
}

DriverPose_t TrackerDriver::GetPose() {
	DriverPose_t pose = { 0 };
	bool SocketActivated = 1;

	char log_str[100];

	// Let's try tracking the waist. Gotta start somewhere, right?
	// If I can track one thing I can track others.
	// First check if device is connected...

	// If everything works here my waist should bug out.
	// This is a good thing, as that means that it's connected with OpenVR.

	if (SocketActivated) {
		pose.poseIsValid = true;
		pose.result = TrackingResult_Running_OK;
		pose.deviceIsConnected = true;
	}
	else {
		pose.poseIsValid = false;
		pose.result = TrackingResult_Uninitialized;
		pose.deviceIsConnected = false;
	}

	int rec_err = 0;
	int BufLen = 12;
	int localAddrSize = sizeof(local);
	char RecvBuf[12];

	rec_err = recvfrom(sock, RecvBuf, BufLen, 0, (SOCKADDR*)& local, &localAddrSize);

	if (rec_err != 12) {
		VRDriverLog()->Log("No rotation detected; skip!");
		
		/*
		ang_x += deg_to_rad((double)Gx / 1310.2);
		ang_y -= deg_to_rad((double)Gy / 1310.2);
		ang_z += deg_to_rad((double)Gz / 1310.2);

		cy = cos(ang_x * 0.5);
		sy = sin(ang_x * 0.5);
		cp = cos(ang_y * 0.5);
		sp = sin(ang_y * 0.5);
		cr = cos(ang_z * 0.5);
		sr = sin(ang_z * 0.5);

		last_pose.qRotation.w = cr * cp * cy + sr * sp * sy;
		last_pose.qRotation.x = sr * cp * cy - cr * sp * sy;
		last_pose.qRotation.y = cr * sp * cy + sr * cp * sy;
		last_pose.qRotation.z = cr * cp * sy - sr * sp * cy;
		*/


		return last_pose;
	} else {
		VRDriverLog()->Log("Updating position!");

		Gx = (short)(RecvBuf[0] << 8 | RecvBuf[1]);
		Gy = (short)(RecvBuf[2] << 8 | RecvBuf[3]);
		Gz = (short)(RecvBuf[4] << 8 | RecvBuf[5]);

		snprintf(log_str, 100, "Gx: %d, Gy: %d, Gz: %d, recverr: %d\n", Gx, Gy, Gz, rec_err);
		VRDriverLog()->Log(log_str);

		HmdQuaternion_t quat;

		quat.x, quat.y, quat.z = 0;
		quat.w = 1;

		pose.qWorldFromDriverRotation = quat;
		pose.qDriverFromHeadRotation = quat;

		ang_x = deg_to_rad((double)Gx / 520.0);
		ang_y = deg_to_rad((double)Gy / 520.0);
		ang_z = deg_to_rad((double)Gz / 520.0);

		// Source: https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles

	https://ahrs.readthedocs.io/en/latest/filters/angular.html
		ang_rate angle_vector(ang_x, ang_y, ang_z);

		omega omega_op(angle_vector);

		double mag = angle_vector.get_magnitude();
		double scale_cos = cos(mag / 2.0);
		double scale_sin = (1 / mag) * sin(mag / 2.0);

		double iMatrixScaled[16] = { 0 };

		double cos_scalar = cos(mag / 2);

		double old_quat[4] = { last_pose.qRotation.w, last_pose.qRotation.x, last_pose.qRotation.y, last_pose.qRotation.z }; // w, x, y, z
		double new_quat[4] = { 1.0, 0.0, 0.0, 0.0 }; // w, x, y, z

		for (int i = 0; i < 16; i++) {
			iMatrixScaled[i] = omega_op.iMatrix[i] * cos_scalar;
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

		last_pose = pose;
		return pose;
	}
}

void TrackerDriver::RunFrame() {
	if (objID != k_unTrackedDeviceIndexInvalid) {
		VRServerDriverHost()->TrackedDevicePoseUpdated(objID, GetPose(), sizeof(DriverPose_t));
	}
}

void TrackerDriver::UDP_init() {

	char log_str[100];

	int iResult;
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	VRDriverLog()->Log("Performing WSAStartup...");
	if (iResult != 0) {
		snprintf(log_str, 100, "WSAStartup failed: %d\n", iResult);
		VRDriverLog()->Log(log_str);
	}
	else {
		
		sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (sock == INVALID_SOCKET) {
			VRDriverLog()->Log("Socket binding failed at step 1!");
		}
		
		local.sin_family = AF_INET;
		local.sin_port = htons(PORT);
		local.sin_addr.s_addr = htonl(INADDR_ANY);

		//int recvbuf_size = 12;
		iResult = ioctlsocket(sock, FIONBIO, &iMode);
		//iResult = setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (char*)&recvbuf_size, sizeof(recvbuf_size));
		iResult = bind(sock, (SOCKADDR*)&local, sizeof(local));

		if (iResult != 0) {
			VRDriverLog()->Log("Socket binding failed at step 2!");
		}
	}
}
