
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

const char* srcIP = "192.168.1.51";
SOCKET sock;
sockaddr_in local;
int local_size = sizeof(local);
WSADATA wsaData;

u_long iMode = 0;

int rec_err = 0;
int BufLen = 25;

char RecvBuf[25];

float Gx, Gy, Gz, Ax, Ay, Az = 0;
double ang_x, ang_y, ang_z, acc_x, acc_y, acc_z = 0;
double hmd_pos[3] = { 0, 1.8, 0};
double waist_to_head = 0.8;

double old_quat[4] = { 1, 0, 0, 0 }; // w, x, y, z
double pose[4] = { 1, 0, 0, 0 };
int ret;

float* gyr_x = (float*)(RecvBuf + 0);
float* gyr_y = (float*)(RecvBuf + 4);
float* gyr_z = (float*)(RecvBuf + 8);
float* tracker_id = (float*)(RecvBuf + 12);

void readUDP() {
	char message[] = "Hello";
	ret = sendto(sock, message, sizeof(message), 0, (struct sockaddr*)&local, sizeof(local));
	while (1) {
		
		
		printf("Bytes sent: %d\n", ret);


		rec_err = recvfrom(sock, RecvBuf, 25, 0, (struct sockaddr*)&local, &local_size);
		printf("Bytes received: %d\n", rec_err);
		printf("%f, %f, %f, %d\n", *gyr_x, *gyr_y, *gyr_z, (int) *tracker_id);

		//printf("Message received.\n");
		//printf("%d, %d, %d\n", Gx, Gy, Gz);
		//printf("Converted to radians: \n");
	}
}

int main() {
	
	printf("Starting program!\n");
	int iResult;
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

	std::chrono::time_point<std::chrono::system_clock> time = std::chrono::system_clock::now();

	if (iResult != 0) {
		printf("WSAStartup failed!\n");
		return 0;
	}
	else {

		sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (sock == INVALID_SOCKET) {
			printf("Socket binding failed at step 1!\n");
			return 0;
		}

		local.sin_family = AF_INET;
		local.sin_port = htons(PORT);
		local.sin_addr.s_addr = inet_addr(srcIP);

		if (iResult != 0) {
			printf("Socket binding failed at step 2!\n");
			return 0;
		}
	}
	std::thread first(readUDP);

	while (1) {
		if (rec_err > 0) {
			


			
		}
		Sleep(1000);
	}
	return 0;
}	

