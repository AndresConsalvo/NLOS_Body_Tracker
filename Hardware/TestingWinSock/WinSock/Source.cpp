
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
int BufLen = 13;

char RecvBuf[13];

short Gx, Gy, Gz, Ax, Ay, Az = 0;
double ang_x, ang_y, ang_z, acc_x, acc_y, acc_z = 0;
double hmd_pos[3] = { 0, 1.8, 0};
double waist_to_head = 0.8;

double old_quat[4] = { 1, 0, 0, 0 }; // w, x, y, z

void readUDP() {
	while (1) {
		int localAddrSize = sizeof(local);
		//printf("Scanning for data\n");
		rec_err = recvfrom(sock, RecvBuf, BufLen, 0, (SOCKADDR*)&local, &localAddrSize);

		//printf("Message received.\n");
		//printf("%d, %d, %d\n", Gx, Gy, Gz);
		//printf("Converted to radians: \n");
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
			double waist_pos[3] = { 0, 0, 0 };
			Gx = (short)(RecvBuf[0] << 8 | RecvBuf[1]) - 4;
			Gy = (short)(RecvBuf[2] << 8 | RecvBuf[3]) + 4;
			Gz = (short)(RecvBuf[4] << 8 | RecvBuf[5]) - 3;

			printf("Raw: %d, %d, %d\n", Gx, Gy, Gz);

			ang_x = deg_to_rad((double)Gx) * 0.01;
			ang_y = deg_to_rad((double)Gy) * 0.01;
			ang_z = deg_to_rad((double)Gz) * 0.01;
			
			ang_rate angle_vector(ang_x, ang_y, ang_z);

			omega omega_op(angle_vector);

			double mag = angle_vector.get_magnitude();
			double scale_cos = cos((mag) / 2.0);
			double scale_sin = (1 / mag) * sin((mag) / 2.0);

			double iMatrixScaled[16] = { 0 };

			double new_quat[4] = { 1.0, 0.0, 0.0, 0.0 }; // w, x, y, z

			for (int i = 0; i < 16; i++) {
				iMatrixScaled[i] = omega_op.iMatrix[i] * scale_cos;
			}

			double sin_scalar = sin(mag / 2);
			sin_scalar = sin_scalar * (1 / mag);

			double omegaMatrixScaled[16] = { 0 };

			for (int i = 0; i < 16; i++) {
				omegaMatrixScaled[i] = omega_op.matrix[i] * sin_scalar;
			}

			double matrix_full[16] = { 0 };

			for (int i = 0; i < 16; i++) {
				matrix_full[i] = omegaMatrixScaled[i] + iMatrixScaled[i];
			}

			for (int i = 0; i < 4; i++) {
				new_quat[i] = (matrix_full[i * 4 + 0] * old_quat[0]) + (matrix_full[i * 4 + 1] * old_quat[1]) + (matrix_full[i * 4 + 2] * old_quat[2]) + (matrix_full[i * 4 + 3] * old_quat[3]);
			}

			double newquatmag = sqrt(pow(new_quat[0], 2) + pow(new_quat[1], 2) + pow(new_quat[2], 2) + pow(new_quat[3], 2));
			old_quat[0] = new_quat[0] / newquatmag;
			old_quat[1] = new_quat[1] / newquatmag;
			old_quat[2] = new_quat[2] / newquatmag;
			old_quat[3] = new_quat[3] / newquatmag;

			double w = old_quat[0];
			double x = old_quat[1];
			double y = old_quat[2];
			double z = old_quat[3];
			double roll, pitch, yaw;
			double t0, t1, t2, t3, t4;
			double vel_x, vel_y, vel_z;

			// from https://automaticaddison.com/how-to-convert-a-quaternion-into-euler-angles-in-python/
			t0 = 2 * (w * x + y * z);
			t1 = 1 - (2 * (x * x + y * y));
			t2 = 2 * (w * y - z * x);
			if (t2 > 1.0) {
				t2 = 1.0;
			} else if (t2 < -1.0) {
				t2 = -1.0;
			}

			t3 = 2 * (w * z + x * y);
			t4 = 1 - 2 * (y * y + z * z);

			vel_x = rad_to_deg(angle_vector.x);
			vel_y = rad_to_deg(angle_vector.y);
			vel_z = rad_to_deg(angle_vector.z);

			roll = atan2(t0, t1);
			pitch = asin(t2);
			yaw = atan2(t3, t4);
			double roll_d = rad_to_deg(roll);
			double pitch_d = rad_to_deg(pitch);
			double yaw_d = rad_to_deg(yaw);

			waist_pos[0] = hmd_pos[0] + waist_to_head * sin(pitch);
			waist_pos[1] = hmd_pos[1] - waist_to_head * cos(pitch);
			waist_pos[2] = hmd_pos[2] + waist_to_head * sin(roll);

			printf("Velocity: ang_x: %f, ang_y: %f, ang_z: %f\n", vel_x, vel_y, vel_z);
			printf("New quat, w: %f, x: %f, y: %f, z: %f\n", w, x, y, z);
			printf("New angle, roll: %f, pitch: %f, yaw: %f\n", roll_d, pitch_d, yaw_d);
			printf("New pos, x: %f, y: %f, z: %f\n", waist_pos[0], waist_pos[1], waist_pos[2]);
			//printf("%f, %f, %f\n", ang_x, ang_y, ang_z);

			


			
		}
		Sleep(1000);
	}
	return 0;
}	

