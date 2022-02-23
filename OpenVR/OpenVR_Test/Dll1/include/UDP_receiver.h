#pragma once

#include <Globals.h>
#include <math_helpers.h>
#include <thread>
#include <WinSock2.h>
#include <Ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

// adapted from https://github.com/m9cd0n9ld/IMU-VR-Full-Body-Tracker/blob/main/steamvr_openvr_driver/driver_imuFBT/headers/UDP.h
class UDP {
public:
	void init();
	void deinit();

private:
	void start();
	void setValue(char* RecvBuf);

	SOCKET sock;
	sockaddr_in local;
	int bytes_read;

	std::thread* receiveThread = nullptr;

	// Keeping track of time
	std::chrono::high_resolution_clock::time_point t_waist_last = std::chrono::high_resolution_clock::now();
	std::chrono::high_resolution_clock::time_point t_lfoot_last = std::chrono::high_resolution_clock::now();
	std::chrono::high_resolution_clock::time_point t_rfoot_last = std::chrono::high_resolution_clock::now();

	std::chrono::high_resolution_clock::time_point t_recv_end = std::chrono::high_resolution_clock::now();

	double elapsed_time_s;
};


