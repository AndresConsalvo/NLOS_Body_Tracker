
#define _USE_MATH_DEFINES

#include <chrono>
#include <iostream>
#include <cmath>
#include <thread>
#include <WinSock2.h>
#include <Ws2tcpip.h>
#include "math_helpers.h"
#pragma comment(lib, "ws2_32.lib")

#define PORT 8080

using namespace std;

const char* srcIP = "127.0.0.1";
WSADATA wsaData;
SOCKET sock;
sockaddr_in local;

int localAddrSize;

int rec_err = 0;
int BufLen = 25;

char RecvBuf[25];


void readUDP() {
	while (1) {
		int localAddrSize = sizeof(local);
		rec_err = recvfrom(sock, RecvBuf, BufLen, 0, (SOCKADDR*)&local, &localAddrSize);
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
		local.sin_addr.s_addr = htonl(INADDR_ANY);

		iResult = bind(sock, (SOCKADDR*)&local, sizeof(local));

		if (iResult != 0) {
			printf("Socket binding failed at step 2!\n");
			return 0;
		}
	}
	std::thread first(readUDP);

	while (1) {
		printf("Bytes read: %d\n", rec_err);
		if (rec_err > 0) {
			
		
			


			
		}
		Sleep(1000);
	}
	return 0;
}	

