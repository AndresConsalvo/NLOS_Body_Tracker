
#define _USE_MATH_DEFINES

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

const char* srcIP = "192.168.1.59";
SOCKET sock;
sockaddr_in local;
WSADATA wsaData;

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

void readUDP() {
	while (1) {
		int localAddrSize = sizeof(local);
		printf("Scanning for data\n");
		rec_err = recvfrom(sock, RecvBuf, BufLen, 0, (SOCKADDR*)&local, &localAddrSize);

		printf("Message received.\n");
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
		local.sin_addr.s_addr = htonl(INADDR_ANY);


		iResult = ioctlsocket(sock, FIONBIO, &iMode);
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
			
			float* acc_x = (float*)RecvBuf;
			float* acc_y = (float*)(RecvBuf + 4);
			float* acc_z = (float*)(RecvBuf + 8);
			float* gyr_x = (float*)(RecvBuf + 12);
			float* gyr_y = (float*)(RecvBuf + 16);
			float* gyr_z = (float*)(RecvBuf + 20);
			printf("Tracker ID: %d\n", RecvBuf[24]);
			printf("Acceleration: %f, %f, %f\n", *acc_x, *acc_y, *acc_z);
			printf("Rotation: %f, %f, %f\n", *gyr_x, *gyr_y, *gyr_z);



			// Below code is for raw data
			/*
			double waist_pos[3] = { 0, 0, 0 };
			Gx = (short)(RecvBuf[0] << 8 | RecvBuf[1]) - 4;
			Gy = (short)(RecvBuf[2] << 8 | RecvBuf[3]) + 4;
			Gz = (short)(RecvBuf[4] << 8 | RecvBuf[5]) - 3;


			printf("Raw: %d, %d, %d\n", Gx, Gy, Gz);

			ang_x = deg_to_rad((double)Gx);
			ang_y = deg_to_rad((double)Gy);
			ang_z = deg_to_rad((double)Gz);

			Vector3_d angle_vector(ang_x, ang_y, ang_z);
			
			
			Quaternion q_t0(pose[0], pose[1], pose[2], pose[3]);
			Matrix44_d omega_Matrix;
			Matrix44_d identity_Matrix;
			omega_Matrix.set_as_Omega_Matrix(angle_vector);
			identity_Matrix.set_as_Identity();
			double ang_mag = angle_vector.getMag();


			double scale_Ident = cos(ang_mag / 2.0);
			double scale_Omega = (1.0 / ang_mag) * sin(ang_mag / 2);
			identity_Matrix.scale_Matrix(scale_Ident);
			omega_Matrix.scale_Matrix(scale_Omega);
			Matrix44_d result = omega_Matrix + identity_Matrix;

			Quaternion q_t1 = result.getNewQuat(q_t0);
			q_t1.normalize();
			printf("x: %f, y: %f, z: %f\n", q_t1.x, q_t1.y, q_t1.z);
			pose[0] = q_t1.w;
			pose[1] = q_t1.x;
			pose[2] = q_t1.y;
			pose[3] = q_t1.z;

			Quaternion posVec, newPos;
			posVec = Quaternion(0, 0, 0.8, 0);
			newPos = q_t1 * posVec * q_t1.GetInverse();
			printf("w: %f, x: %f, y: %f, z: %f\n", newPos.w, newPos.x, newPos.y, newPos.z);

			waist_pos[0] = hmd_pos[0] + newPos.x;
			waist_pos[1] = hmd_pos[1] - newPos.y;
			waist_pos[2] = hmd_pos[2] + newPos.z;


			printf("x: %f, y: %f, z: %f\n", waist_pos[0], waist_pos[1], waist_pos[2]);
			*/
			


			
		}
		Sleep(1000);
	}
	return 0;
}	

