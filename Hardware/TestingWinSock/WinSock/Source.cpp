
#define _USE_MATH_DEFINES

#include <chrono>
#include <iostream>
#include <cmath>
#include <thread>
#include <WinSock2.h>
#include <Ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#define PORT 20000

#define deg_to_rad(x) (x * M_PI/180.0)

using namespace std;

const char* srcIP = "192.168.1.59";
SOCKET sock;
sockaddr_in local;
WSADATA wsaData;

u_long iMode = 0;

int rec_err = 0;
int BufLen = 12;

char RecvBuf[12];

short Gx, Gy, Gz, Ax, Ay, Az = 0;
double ang_x, ang_y, ang_z, acc_x, acc_y, acc_z = 0;

void readUDP() {
	while (1) {
		int localAddrSize = sizeof(local);
		printf("Scanning for data\n");
		rec_err = recvfrom(sock, RecvBuf, BufLen, 0, (SOCKADDR*)&local, &localAddrSize);

		printf("Message received.\n");
		printf("%d, %d, %d\n", Gx, Gy, Gz);
		printf("Converted to radians: \n");
	}
}

int main() {
	std::thread first(readUDP);

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
		local.sin_addr.s_addr = htonl(INADDR_ANY);


		iResult = ioctlsocket(sock, FIONBIO, &iMode);
		iResult = bind(sock, (SOCKADDR*)&local, sizeof(local));

		if (iResult != 0) {
			printf("Socket binding failed at step 2!\n");
			return 0;
		}
	}

	while (1) {
		if (rec_err > 0) {
			Gx = (short)(RecvBuf[0] << 8 | RecvBuf[1]);
			Gy = (short)(RecvBuf[2] << 8 | RecvBuf[3]);
			Gz = (short)(RecvBuf[4] << 8 | RecvBuf[5]);

			printf("%d, %d, %d\n", Gx, Gy, Gz);

			ang_x = deg_to_rad((double)Gx) * 0.01;
			ang_y = deg_to_rad((double)Gy) * 0.01;
			ang_z = deg_to_rad((double)Gz) * 0.01;
			//printf("%f, %f, %f\n", ang_x, ang_y, ang_z);
		}
	}
	return 0;
}	

