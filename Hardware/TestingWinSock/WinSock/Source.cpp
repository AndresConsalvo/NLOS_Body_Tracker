#include <iostream>
#include <WinSock2.h>
#include <Ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#define PORT 20000


using namespace std;

int main() {

	const char* srcIP = "192.168.1.59";
	SOCKET sock;
	sockaddr_in local;
	WSADATA wsaData;

	u_long iMode = 1;

	printf("Starting program!\n");
	int iResult;
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

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

	int rec_err = 0;
	int BufLen = 12;
	int localAddrSize = sizeof(local);
	char RecvBuf[12];

	int test = 0;

	short Gx, Gy, Gz, Ax, Ay, Az;
	double ang_x, ang_y, ang_z, acc_x, acc_y, acc_z;

	while (1) {
		rec_err = recvfrom(sock, RecvBuf, BufLen, 0, (SOCKADDR*)&local, &localAddrSize);

		if (rec_err > 0) {
			Gx = (short)(RecvBuf[0] << 8 | RecvBuf[1]);
			Gy = (short)(RecvBuf[2] << 8 | RecvBuf[3]);
			Gz = (short)(RecvBuf[4] << 8 | RecvBuf[5]);
			
			printf("Message received.\n");
			printf("%d, %d, %d\n", Gx, Gy, Gz);
			printf("Converted to degrees: \n");

			ang_x = ((double)Gx / 131.2);
			ang_y = ((double)Gy / 131.2);
			ang_z = ((double)Gz / 131.2);

			printf("%f, %f, %f\n", ang_x, ang_y, ang_z);

		}
	}
	return 0;
}	