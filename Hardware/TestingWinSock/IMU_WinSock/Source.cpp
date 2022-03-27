
#define _USE_MATH_DEFINES
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma pack(n)

#include <chrono>
#include <iostream>
#include <cmath>
#include <thread>
#include <WinSock2.h>
#include <Ws2tcpip.h>
#include "math_helpers.h"
#pragma comment(lib, "ws2_32.lib")

#define PORT 20000

using namespace std;


WSADATA wsaData;

SOCKET sock;
sockaddr_in local;
int localAddrSize = sizeof(local);

const uint16_t serverPort = 4242;

std::chrono::high_resolution_clock::time_point last_received;
std::chrono::high_resolution_clock::time_point current_time;
double elapsed_time_s;


int bytes_read = 0;
int BufLen = 255;

char RecvBuf[255];

double hmd_pos[3] = { 0, 1.8, 0};
double hmd_quat[4] = { 1.0, 0.0, 0.0, 0.0 };


#pragma pack(push, 1)
struct data_pkg {
	uint8_t header;
	float x = 0.0;
	float y = 0.0;
	float z = 0.0;
	float qw = 1.0;
	float qx = 0.0;
	float qy = 0.0;
	float qz = 0.0;
	uint8_t tracker_id = 0;
	uint8_t footer;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct ping_pkg {
	uint8_t header = 'P';
	uint8_t msg = 45;
	uint8_t footer = 'p';
};
#pragma pack(pop)

ping_pkg* server_response;
data_pkg* payload;
bool broadcast_en = true;
bool server_connected = false;



void readUDP() {
	while (1) {
		memset(RecvBuf, 0, sizeof(RecvBuf));
		//printf("Scanning for data\n");
		bytes_read = recvfrom(sock, RecvBuf, BufLen, 0, (SOCKADDR*)&local, &localAddrSize);
		printf("%d\n", bytes_read);

		if (bytes_read == sizeof(ping_pkg)) {
			server_response = (ping_pkg*)RecvBuf;
			printf("Server responded, ending broadcast.\n");

			if (server_response->header == 'P' && server_response->footer == 'p') {
				broadcast_en = false;
				server_connected = true;
			}
		}
		
		if (bytes_read == sizeof(data_pkg)) {
			payload = (data_pkg*)RecvBuf;

			if (payload->header == 'T' && payload->footer == 't') {
				printf("Header and footer: %c %c\n", payload->header, payload->footer);
				printf("Tracker ID: %d\n", payload->tracker_id);
				printf("Rotation [w, x, y, z] = [%f, %f, %f, %f]\n", payload->qw, payload->qx, payload->qy, payload->qz);
				printf("Position [x, y, z] = [%f, %f, %f]\n", payload->x, payload->y, payload->z);
				last_received = std::chrono::high_resolution_clock::now();
			}
		}
		
	}
}

void reconnectUDP() {
	while (1) {
		if (broadcast_en) {
			ping_pkg ping;
			sendto(sock, (char*)&ping, sizeof(ping), 0, (sockaddr*)&local, localAddrSize);
			printf("Broadcasting!\n");
		}
		Sleep(1000);
	}
}

int main() {
	current_time = std::chrono::high_resolution_clock::now();
	last_received = std::chrono::high_resolution_clock::now();

	printf("Starting program!\n");
	int iResult;
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (iResult != 0) {
		printf("WSAStartup failed!\n");
		return 0;
	}
	else {

		local.sin_family = AF_INET;
		local.sin_port = htons(serverPort);
		local.sin_addr.s_addr = inet_addr("127.0.0.1");

		sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

		int enable = 1;
		setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char*)&enable, sizeof(enable));


		if (sock == INVALID_SOCKET) {
			printf("Socket binding failed at step 1!\n");
			return 0;
		}
		//iResult = bind(sock, (sockaddr*)&local, sizeof(local));
		if (iResult != 0) {
			printf("Socket binding failed at step 2!\n");
			return 0;
		}
	}

	// Start threads
	std::thread first(readUDP);
	std::thread second(reconnectUDP);

	while (1) {
		if (elapsed_time_s > 5.0) {
			broadcast_en = true;
			server_connected = false;
		}
		
		if (server_connected) {
			printf("Sending hmd data\n");
			data_pkg hmd_payload;
			hmd_payload.header = (uint8_t)'H';
			hmd_payload.x = hmd_pos[0];
			hmd_payload.y = hmd_pos[1];
			hmd_payload.z = hmd_pos[2];
			hmd_payload.qw = hmd_quat[0];
			hmd_payload.qx = hmd_quat[1];
			hmd_payload.qy = hmd_quat[2];
			hmd_payload.qz = hmd_quat[3];
			hmd_payload.tracker_id = 0;
			hmd_payload.footer = (uint8_t)'h';

			sendto(sock, (char*)&hmd_payload, sizeof(hmd_payload), 0, (sockaddr*)&local, localAddrSize);

			current_time = std::chrono::high_resolution_clock::now();
			elapsed_time_s = std::chrono::duration<double, std::milli>(current_time - last_received).count() / 1000.0;
			printf("%f\n", elapsed_time_s);

			Sleep(1000);
		}

	}
	return 0;
}	

