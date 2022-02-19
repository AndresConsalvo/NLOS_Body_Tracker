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
		return last_pose;

	} else {
		VRDriverLog()->Log("Updating position!");
		
		// Need to move the gyroscope receive and calculate somewhere else.
		Gx = (short)(RecvBuf[0] << 8 | RecvBuf[1]);
		Gy = (short)(RecvBuf[2] << 8 | RecvBuf[3]);
		Gz = (short)(RecvBuf[4] << 8 | RecvBuf[5]);

		snprintf(log_str, 100, "Gx: %d, Gy: %d, Gz: %d, recverr: %d\n", Gx, Gy, Gz, rec_err);
		VRDriverLog()->Log(log_str);

		ang_x = deg_to_rad((double)Gx / 520.0);
		ang_y = deg_to_rad((double)Gy / 520.0);
		ang_z = deg_to_rad((double)Gz / 520.0);
	
		ang_rate angle_vector(ang_x, ang_y, ang_z);
		pose = getNewPose(last_pose, angle_vector);

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

		iResult = ioctlsocket(sock, FIONBIO, &iMode);
		iResult = bind(sock, (SOCKADDR*)&local, sizeof(local));

		if (iResult != 0) {
			VRDriverLog()->Log("Socket binding failed at step 2!");
		}
	}
}

void TrackerDriver::setIndex(int limbIndex) {
	this->TrackerIndex = limbIndex;
}
