#pragma once
#include <UDP_receiver.h>


#define PORT 20000
const char* srcIP = "192.168.1.59";

char log_str[100];


// Initializes UDP socket
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
		local.sin_port = htons(serverPort);
		local.sin_addr.s_addr = inet_addr("127.0.0.1");

		sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

		int enable = 1;
		int iTimeout = 100;
		setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char*)&enable, sizeof(enable));
		setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&iTimeout, sizeof(iTimeout));

		SocketActivated = true;
		if (sock == INVALID_SOCKET) {
			WSACleanup();
			SocketActivated = false;
			vr::VRDriverLog()->Log("Socket startup failed!");
		}

		receiveThread = new std::thread(&UDP::start, this);
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
	while (SocketActivated) {
		vr::VRDriverLog()->Log("Receiving data!");
		bytes_read = recvfrom(sock, RecvBuf, 25, 0, (sockaddr*)&local, &localAddrSize);
		snprintf(log_str, 100, "Read %d bytes\n", bytes_read);
		VRDriverLog()->Log(log_str);
		if (bytes_read == 16) {
			VRDriverLog()->Log("Setting value!");
		} else if (bytes_read == 17) {
			VRDriverLog()->Log("Setting body measurements!");
		} else {
			vr::VRDriverLog()->Log("No data received!");
		}
		t_recv_end = std::chrono::high_resolution_clock::now();
	}
}

void UDP::setValue(data_pkg* payload) {
	DriverPose_t* pose = &hmd_pose;

	switch (payload->tracker_id) {
	case CHEST:
		break;
	case WAIST:
		pose = &waist_pose;
		break;
	case LKNEE:
		pose = &lknee_pose;
		break;
	case RKNEE:
		pose = &rknee_pose;
		break;
	case LFOOT:
		pose = &lfoot_pose;
		break;
	case RFOOT:
		pose = &rfoot_pose;
		break;
	default:
		vr::VRDriverLog()->Log("Invalid tracker ID received! Default to HMD");
		pose = &hmd_pose;
		return;
	}

	pose->qRotation.w = payload->qw;
	pose->qRotation.x = payload->qx;
	pose->qRotation.y = payload->qy;
	pose->qRotation.z = payload->qz;

	pose->vecPosition[0] = payload->x;
	pose->vecPosition[1] = payload->y;
	pose->vecPosition[2] = payload->z;

	return;
}
