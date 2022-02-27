#pragma once
#include <UDP_receiver.h>


#define PORT 20000
const char* srcIP = "192.168.1.59";

char log_str[100];



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

	char RecvBuf[13];
	int localAddrSize = sizeof(local);
	while (SocketActivated) {
		vr::VRDriverLog()->Log("Receiving data!");
		bytes_read = recvfrom(sock, RecvBuf, 13, 0, (sockaddr*)&local, &localAddrSize);
		vr::VRDriverLog()->Log("Data received!");
		if (bytes_read == 13) {
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
	short tracker_ID = RecvBuf[12];
	snprintf(log_str, 100, "TrackerID: %d\n", tracker_ID);
	VRDriverLog()->Log(log_str);

	// Because of how IMU is positioned, data received does not correspond with axis.
	short Gz = (short)(RecvBuf[0] << 8 | RecvBuf[1]);
	short Gy = (short)(RecvBuf[2] << 8 | RecvBuf[3]);
	short Gx = (short)(RecvBuf[4] << 8 | RecvBuf[5]);

	snprintf(log_str, 100, "Gx: %d, Gy: %d, Gz: %d\n", Gx, Gy, Gz);
	VRDriverLog()->Log(log_str);

	double ang_x = -deg_to_rad((double)Gx / 524.8);
	double ang_y = -deg_to_rad((double)Gy / 524.8);
	double ang_z = -deg_to_rad((double)Gz / 524.8);
	snprintf(log_str, 100, "ang_x: %f, ang_y: %f, ang_z: %f\n", ang_x, ang_y, ang_z);
	VRDriverLog()->Log(log_str);

	ang_rate angle_vector(ang_x, ang_y, ang_z);

	switch (tracker_ID) {
	case WAIST:
		t_waist_last = std::chrono::high_resolution_clock::now();
		elapsed_time_s = std::chrono::duration<double, std::milli>(t_waist_last - t_recv_end).count() / 1000.0;
		getNewPose(WAIST, angle_vector, elapsed_time_s);

		waist_pose.poseIsValid = true;
		waist_pose.result = TrackingResult_Running_OK;
		waist_pose.deviceIsConnected = true;
		break;
	case LFOOT:
		t_lfoot_last = std::chrono::high_resolution_clock::now();
		elapsed_time_s = std::chrono::duration<double, std::milli>(t_lfoot_last - t_recv_end).count() / 1000.0;
		getNewPose(LFOOT, angle_vector, elapsed_time_s);

		lfoot_pose.poseIsValid = true;
		lfoot_pose.result = TrackingResult_Running_OK;
		lfoot_pose.deviceIsConnected = true;
		break;
	case RFOOT:
		t_rfoot_last = std::chrono::high_resolution_clock::now();
		elapsed_time_s = std::chrono::duration<double, std::milli>(t_rfoot_last - t_recv_end).count() / 1000.0;
		getNewPose(RFOOT, angle_vector, elapsed_time_s);

		rfoot_pose.poseIsValid = true;
		rfoot_pose.result = TrackingResult_Running_OK;
		rfoot_pose.deviceIsConnected = true;
		
	default:
		break;
	}

}