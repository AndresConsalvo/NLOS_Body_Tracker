#pragma once
#include <UDP_receiver.h>


#define PORT 20000
const char* srcIP = "192.168.1.59";

char log_str[100];
char RecvBuf[25];

float* acc_x = (float*)RecvBuf;
float* acc_y = (float*)(RecvBuf + 4);
float* acc_z = (float*)(RecvBuf + 8);
float* gyr_x = (float*)(RecvBuf + 12);
float* gyr_y = (float*)(RecvBuf + 16);
float* gyr_z = (float*)(RecvBuf + 20);


void UDP::init() {
	WSADATA wsaData;
	int iResult;

	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	vr::VRDriverLog()->Log("Starting UDP thread!");
	
	if (iResult != 0) {
		vr::VRDriverLog()->Log("Startup failed!");
		WSACleanup();
		SocketActivated = false;
	} else {
		
		local.sin_family = AF_INET;
		local.sin_port = htons(PORT);
		local.sin_addr.s_addr = htonl(INADDR_ANY);

		sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

		if (sock != INVALID_SOCKET) {
			iResult = bind(sock, (SOCKADDR*)&local, sizeof(local));
			

			if (iResult != SOCKET_ERROR) {
				SocketActivated = true;
				receiveThread = new std::thread(&UDP::start, this);
			}
			else {
				SocketActivated = false;
				WSACleanup();
				vr::VRDriverLog()->Log("Socket binding failed!");
			}
		} else {
			WSACleanup();
			SocketActivated = false;
			vr::VRDriverLog()->Log("Socket startup failed!");
		}
	}
}

void UDP::deinit() {
	if (SocketActivated) {
		SocketActivated = false;
		if (receiveThread) {
			receiveThread->join();
			delete receiveThread;
			receiveThread = nullptr;
		}
		closesocket(sock);
		WSACleanup();
	}
}

void UDP::start() {

	
	int localAddrSize = sizeof(local);
	while (SocketActivated) {
		vr::VRDriverLog()->Log("Receiving data!");
		bytes_read = recvfrom(sock, RecvBuf, 25, 0, (sockaddr*)&local, &localAddrSize);
		snprintf(log_str, 100, "Read %d bytes\n", bytes_read);
		VRDriverLog()->Log(log_str);
		if (bytes_read == 25) {
			VRDriverLog()->Log("Setting value!");
			setValue((char*)RecvBuf);
		} else {
			vr::VRDriverLog()->Log("No data received!");
		}
		t_recv_end = std::chrono::high_resolution_clock::now();
	}
}

void UDP::setValue(char* RecvBuf) {
	// Need to move the gyroscope receive and calculate somewhere else.

	// short tracker_ID = (short)(RecvBuf[6]);
	short tracker_ID = RecvBuf[25];
	snprintf(log_str, 100, "TrackerID: %d\n", tracker_ID);
	VRDriverLog()->Log(log_str);

	/*
	short Gx = (short)(RecvBuf[0] << 8 | RecvBuf[1]);
	short Gy = (short)(RecvBuf[2] << 8 | RecvBuf[3]);
	short Gz = (short)(RecvBuf[4] << 8 | RecvBuf[5]);

	snprintf(log_str, 100, "Gx: %d, Gy: %d, Gz: %d\n", Gx, Gy, Gz);
	VRDriverLog()->Log(log_str);
	*/
	// Because of how IMU is positioned, data received does not correspond with axis.


	double ang_x = *gyr_x;
	double ang_y = *gyr_y;
	double ang_z = *gyr_z;

	snprintf(log_str, 100, "ang_x: %f, ang_y: %f, ang_z: %f\n", ang_x, ang_y, ang_z);
	VRDriverLog()->Log(log_str);

	Vector3_d angle_vector(ang_x, -ang_y, -ang_z);

	if (tracker_ID == 4) {
		reset_trackers();
	} else {
		switch (tracker_ID) {
		case WAIST:
			VRDriverLog()->Log("Posing waist\n");

			t_waist_last = std::chrono::high_resolution_clock::now();
			elapsed_time_s = std::chrono::duration<double, std::milli>(t_waist_last - t_recv_end).count() / 1000.0;
			getNewPose(WAIST, angle_vector, elapsed_time_s);

			waist_pose.poseIsValid = true;
			waist_pose.result = TrackingResult_Running_OK;
			waist_pose.deviceIsConnected = true;
			break;
		case LFOOT:
			VRDriverLog()->Log("Posing left foot\n");
			t_lfoot_last = std::chrono::high_resolution_clock::now();
			elapsed_time_s = std::chrono::duration<double, std::milli>(t_lfoot_last - t_recv_end).count() / 1000.0;
			getNewPose(LFOOT, angle_vector, elapsed_time_s);

			lfoot_pose.poseIsValid = true;
			lfoot_pose.result = TrackingResult_Running_OK;
			lfoot_pose.deviceIsConnected = true;
			break;
		case RFOOT:
			VRDriverLog()->Log("Posing right foot\n");

			t_rfoot_last = std::chrono::high_resolution_clock::now();
			elapsed_time_s = std::chrono::duration<double, std::milli>(t_rfoot_last - t_recv_end).count() / 1000.0;
			getNewPose(RFOOT, angle_vector, elapsed_time_s);

			rfoot_pose.poseIsValid = true;
			rfoot_pose.result = TrackingResult_Running_OK;
			rfoot_pose.deviceIsConnected = true;
	
		default:
			break;
		}

		updateSkeleton();
	}

}

void UDP::reset_trackers() {
	VRDriverLog()->Log("Resetting tracker pos!\n");

	neck_pose.vecPosition[0] = 0.0;
	neck_pose.vecPosition[1] = 1.6;
	neck_pose.vecPosition[2] = -Head_to_Neck;

	waist_pose.vecPosition[0] = 0.0;
	waist_pose.vecPosition[1] = neck_pose.vecPosition[1] - Neck_to_Waist;
	waist_pose.vecPosition[2] = neck_pose.vecPosition[2];

	lfoot_pose.vecPosition[0] = lhip_pose.vecPosition[0];
	lfoot_pose.vecPosition[1] = lhip_pose.vecPosition[1] - Hip_to_Foot_len_m;
	lfoot_pose.vecPosition[2] = lhip_pose.vecPosition[2];

	rfoot_pose.vecPosition[0] = rhip_pose.vecPosition[0];
	rfoot_pose.vecPosition[1] = rhip_pose.vecPosition[1] - Hip_to_Foot_len_m;
	rfoot_pose.vecPosition[2] = rhip_pose.vecPosition[2];

	lfoot_pose.qRotation.w = 1.0;
	lfoot_pose.qRotation.x = 0;
	lfoot_pose.qRotation.y = 0;
	lfoot_pose.qRotation.z = 0;


	waist_pose.qRotation.w = 1.0;
	waist_pose.qRotation.x = 0;
	waist_pose.qRotation.y = 
	waist_pose.qRotation.z = 0;

	lfoot_pose.qRotation.w = 1.0;
	lfoot_pose.qRotation.x = 0;
	lfoot_pose.qRotation.y = 0;
	lfoot_pose.qRotation.z = 0;

	rfoot_pose.qRotation.w = 1.0;
	rfoot_pose.qRotation.x = 0;
	rfoot_pose.qRotation.y = 0;
	rfoot_pose.qRotation.z = 0;

	return;
}