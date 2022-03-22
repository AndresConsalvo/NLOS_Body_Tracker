
#define _USE_MATH_DEFINES
#define _WINSOCK_DEPRECATED_NO_WARNINGS

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



SOCKET sock;
sockaddr_in local;
int localAddrSize = sizeof(local);
WSADATA wsaData;

SOCKET socketB;
sockaddr_in localB;
int locallenB = sizeof(localB);
const uint16_t broadPort = 4242;
bool toBroadcast = true;

std::chrono::high_resolution_clock::time_point last_received;
std::chrono::high_resolution_clock::time_point current_time;
double elapsed_time_s;


u_long iMode = 0;

int rec_err = 0;
int BufLen = 255;

char RecvBuf[255];

float Gx, Gy, Gz, Ax, Ay, Az = 0;
double ang_x, ang_y, ang_z, acc_x, acc_y, acc_z = 0;
double hmd_pos[3] = { 0, 1.8, 0};
double waist_to_head = 0.8;

double old_quat[4] = { 1, 0, 0, 0 }; // w, x, y, z
double pose[4] = { 1, 0, 0, 0 };

int port = 0;


void readUDP() {
	while (1) {
		//printf("Scanning for data\n");
		rec_err = recvfrom(sock, RecvBuf, BufLen, 0, (SOCKADDR*)&local, &localAddrSize);
		
		if (rec_err >= 0) {
			printf("%d\n", rec_err);
			last_received = std::chrono::high_resolution_clock::now();
		}
		//printf("Message received.\n");
		//printf("%d, %d, %d\n", Gx, Gy, Gz);
		//printf("Converted to radians: \n");
	}
}

void reconnectUDP() {
	while (1) {
		elapsed_time_s = std::chrono::duration<double, std::milli>(current_time - last_received).count() / 1000.0;
		if (elapsed_time_s > 1.0) {
			//printf("Hey, this isn't connected!\n");
			char test[50];


			sendto(socketB, (char*)&port, sizeof(port), 0, (sockaddr*)&localB, locallenB);
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

		localB.sin_family = AF_INET;
		inet_pton(AF_INET, "255.255.255.255", &localB.sin_addr.s_addr);
		localB.sin_port = htons(broadPort);
		socketB = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		char broadcast = 1;
		setsockopt(socketB, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast));


		local.sin_family = AF_INET;
		local.sin_port = htons(0);
		local.sin_addr.s_addr = inet_addr("0.0.0.0");

		sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

		int enable = 1;
		setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char*)&enable, sizeof(enable));


		iResult = bind(sock, (sockaddr*)&local, sizeof(local));

		struct sockaddr_in sin;
		socklen_t len = sizeof(sin);
		getsockname(sock, (struct sockaddr*)&sin, &len);
		port = ntohs(sin.sin_port);

		if (sock == INVALID_SOCKET) {
			printf("Socket binding failed at step 1!\n");
			return 0;
		}
		//iResult = bind(sock, (sockaddr*)&local, sizeof(local));
		if (iResult != 0) {
			printf("Socket binding failed at step 2!\n");
			return 0;
		}

		printf("%u\n", port);
	}

	std::thread first(readUDP);
	std::thread second(reconnectUDP);

	while (1) {
		current_time = std::chrono::high_resolution_clock::now();
		
		printf("Bytes read: %d\n", rec_err);
		if (rec_err > 0) {
			
			float* gyr_x = (float*)(RecvBuf + 0);
			float* gyr_y = (float*)(RecvBuf + 4);
			float* gyr_z = (float*)(RecvBuf + 8);
			short tracker_id = (short) (*(float*)(RecvBuf + 12));
			printf("Tracker ID: %d\n", tracker_id);
			printf("Rotation: %f, %f, %f\n", *gyr_x, *gyr_y, *gyr_z);


			
		}
		Sleep(1000);
	}
	return 0;
}	

