#pragma once
#include <UDP_receiver.h>

// For logging purposes (no use in normal operation)
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
		//setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&iTimeout, sizeof(iTimeout));

		SocketActivated = true;
		if (sock == INVALID_SOCKET) {
			WSACleanup();
			SocketActivated = false;
			vr::VRDriverLog()->Log("Socket startup failed!");
		}

		receiveThread = new std::thread(&UDP::start, this);
		broadcastThread = new std::thread(&UDP::start_broadcasting, this);
	}
}

void UDP::deinit() {
	if (SocketActivated) {
		SocketActivated = false;
		if (receiveThread) {
			receiveThread->join();
			delete receiveThread;

			broadcastThread->join();
			delete broadcastThread;

			receiveThread = nullptr;
			broadcastThread = nullptr;
		}
		closesocket(sock);
		WSACleanup();
	}
}



void UDP::start() {
	while (SocketActivated) {

		//vr::VRDriverLog()->Log("Receiving data!");
		bytes_read = recvfrom(sock, RecvBuf, BufLen, 0, (sockaddr*)&local, &localAddrSize);
		//snprintf(log_str, 100, "Read %d bytes\n", bytes_read);
		//VRDriverLog()->Log(log_str);
			
		if (bytes_read == sizeof(data_pkg)) {
			payload = (data_pkg*)(RecvBuf);
			if (payload->header == 'T' && payload->footer == 't') {
				//VRDriverLog()->Log("Setting tracker position and rotation!");
				setValue(payload);
			}
		} else if (bytes_read == sizeof(ping_pkg)) {
			server_response = (ping_pkg*)(RecvBuf);

			if (server_response->header == 'P' && server_response->footer == 'p') {
				VRDriverLog()->Log("Server responded, ending broadcast.\n");
				this->broadcast_en = false;
			}
			if (server_response->header == 'C' && server_response->footer == 'c') {
				if (server_response->msg == 0) {
					//VRDriverLog()->Log("HMD data requested!");
					TrackedDevicePose_t device_pose[10];
					VRServerDriverHost()->GetRawTrackedDevicePoses(0, device_pose, 10);
					HmdMatrix34_t space_matrix = device_pose[k_unTrackedDeviceIndex_Hmd].mDeviceToAbsoluteTracking;
					Quaternion hmd_quat = getQuaternionFromHMD(space_matrix);

					data_pkg hmd_data;

					hmd_data.header = 'H';
					hmd_data.x = space_matrix.m[0][3];
					hmd_data.y = space_matrix.m[1][3];
					hmd_data.z = space_matrix.m[2][3];
					hmd_data.qw = hmd_quat.w;
					hmd_data.qx = hmd_quat.x;
					hmd_data.qy = hmd_quat.y;
					hmd_data.qz = hmd_quat.z;
					hmd_data.tracker_id = 0;
					hmd_data.footer = 'h';
					
					sendto(sock, (char*)&hmd_data, sizeof(hmd_data), 0, (sockaddr*)&local, localAddrSize);
				}
			}
		} else {
			//vr::VRDriverLog()->Log("No data received!");
		}
		t_recv_end = std::chrono::high_resolution_clock::now();
	}
}

void UDP::start_broadcasting() {
	while (1) {
		if (this->broadcast_en) {
			ping_pkg ping;
			sendto(sock, (char*)&ping, sizeof(ping), 0, (sockaddr*)&local, localAddrSize);
			VRDriverLog()->Log("Broadcasting!\n");
			Sleep(1000);
		} else {
			TrackedDevicePose_t device_pose[10];
			VRServerDriverHost()->GetRawTrackedDevicePoses(0, device_pose, 10);
			HmdMatrix34_t space_matrix = device_pose[k_unTrackedDeviceIndex_Hmd].mDeviceToAbsoluteTracking;
			Quaternion hmd_quat = getQuaternionFromHMD(space_matrix);

			data_pkg hmd_data;

			hmd_data.header = 'H';
			hmd_data.x = space_matrix.m[0][3];
			hmd_data.y = space_matrix.m[1][3];
			hmd_data.z = space_matrix.m[2][3];
			hmd_data.qw = hmd_quat.w;
			hmd_data.qx = hmd_quat.x;
			hmd_data.qy = hmd_quat.y;
			hmd_data.qz = hmd_quat.z;
			hmd_data.tracker_id = 0;
			hmd_data.footer = 'h';

			sendto(sock, (char*)&hmd_data, sizeof(hmd_data), 0, (sockaddr*)&local, localAddrSize);
			Sleep(10);
		}
	}
}

void UDP::setValue(data_pkg* payload) {
	DriverPose_t* pose = &hmd_pose;

	switch (payload->tracker_id) {
	case CHEST:
		pose = &chest_pose;
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

	pose->poseIsValid = true;
	
	pose->deviceIsConnected = true;

	pose->qRotation.w = payload->qw;
	pose->qRotation.x = payload->qx;
	pose->qRotation.y = payload->qy;
	pose->qRotation.z = payload->qz;

	pose->vecPosition[0] = payload->x;
	pose->vecPosition[1] = payload->y;
	pose->vecPosition[2] = payload->z;

	return;
}


