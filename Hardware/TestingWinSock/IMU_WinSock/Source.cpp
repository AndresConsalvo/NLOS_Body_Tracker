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

float hmd_pos[3] = { 0.0, 1.8, 0.0};
float hmd_quat[4] = { -0.776, 0.249, 0.093, 0.573 };


#pragma pack(push, 1)
struct data_pkg {
	uint8_t header = 'T';
	float x = 0.0;
	float y = 0.0;
	float z = 0.0;
	float qw = 1.0;
	float qx = 0.0;
	float qy = 0.0;
	float qz = 0.0;
	uint8_t tracker_id = 0;
	uint8_t footer = 't';
};
#pragma pack(pop)

#pragma pack(push, 1)
struct cmd_pkg {
	uint8_t header = 'P';
	uint8_t msg = 45;
	uint8_t footer = 'p';
};
#pragma pack(pop)


cmd_pkg* server_response;
data_pkg* payload;
bool broadcast_en = true;
bool server_connected = false;

void readUDP() {
	while (1) {
		memset(RecvBuf, 0, sizeof(RecvBuf));
		//printf("Scanning for data\n");
		bytes_read = recvfrom(sock, RecvBuf, BufLen, 0, (SOCKADDR*)&local, &localAddrSize);
		//printf("%s\n", RecvBuf);

		if (bytes_read == sizeof(cmd_pkg)) {
			server_response = (cmd_pkg*)RecvBuf;
			//printf("Server responded\n");

			if (server_response->header == 'P' && server_response->footer == 'p') {
				last_received = std::chrono::high_resolution_clock::now();
				printf("Server responded, ending broadcast.\n");
				broadcast_en = false;
				server_connected = true;
			}

			if (server_response->header == 'C' && server_response->footer == 'c') {
				last_received = std::chrono::high_resolution_clock::now();
				data_pkg hmd_payload;

				switch (server_response->msg) {
				case 0:
					//printf("Sending hmd data\n");

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
					break;
				default:
					printf("Invalid command\n");
					break;
				}
			}
		}
		
		if (bytes_read == sizeof(data_pkg)) {
			
			//printf("Data received.\n");
			payload = (data_pkg*)RecvBuf;

			if (payload->header == 'T' && payload->footer == 't') {
				last_received = std::chrono::high_resolution_clock::now();
				printf("Header and footer: %c %c\n", payload->header, payload->footer);
				printf("Tracker ID: %d\n", payload->tracker_id);
				printf("Rotation [w, x, y, z] = [%f, %f, %f, %f]\n", payload->qw, payload->qx, payload->qy, payload->qz);
				printf("Position [x, y, z] = [%f, %f, %f]\n", payload->x, payload->y, payload->z);

				if (payload->tracker_id == 6) {
					system("CLS");
				}
			}
		}
	}
}

void reconnectUDP() {
	while (1) {
		if (broadcast_en) {
			cmd_pkg ping;
			sendto(sock, (char*)&ping, sizeof(ping), 0, (sockaddr*)&local, localAddrSize);
			printf("Broadcasting!\n");
			Sleep(1000);
		} else {
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
			Sleep(20);
		}
		
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
		int iTimeout = 100;
		setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char*)&enable, sizeof(enable));
		setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&iTimeout, sizeof(iTimeout));
		// Set timeout


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
		current_time = std::chrono::high_resolution_clock::now();
		elapsed_time_s = std::chrono::duration<double, std::milli>(current_time - last_received).count() / 1000.0;
		if (elapsed_time_s > 5.0) {
			broadcast_en = true;
			server_connected = false;
		}

	}
	return 0;
}	

