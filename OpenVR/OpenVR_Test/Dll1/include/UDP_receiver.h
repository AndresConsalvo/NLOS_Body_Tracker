#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <Globals.h>
#include <math_helpers.h>
#include <thread>
#include <WinSock2.h>
#include <Ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#define tracker_data 0
#define set_config 1
#define calibrate 2



// adapted from https://github.com/m9cd0n9ld/IMU-VR-Full-Body-Tracker/blob/main/steamvr_openvr_driver/driver_imuFBT/headers/UDP.h
class UDP {
public:
	void init();
	void deinit();

private:
	void start();
	void setValue(char* RecvBuf);
	void reset_trackers();

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


